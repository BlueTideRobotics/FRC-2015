 #include <cstdint>
#include "WPILib.h"

#define FUCK_EVERYTHING 42

double map(double value,double high, double low)
{
	return ((value+fabs(low))/((high+fabs(low))/2))-1;
}
double trueMap(double val,double valHigh,double valLow,double newHigh, double newLow)
{
	double midVal=((valHigh-valLow)/2)+valLow;
	double newMidVal=((newHigh-newLow)/2)+newLow;
	double ratio=(newHigh-newLow)/(valHigh-valLow);
	return (((val-midVal)*ratio)+newMidVal);
}
double inverseMap(double value, double high, double low)
{
	return(((value+1)*((high+fabs(low))/2)))+low;
}
double constrain(double value,double highConstraint,double lowConstraint)
{
	if(value>highConstraint)
		value=highConstraint;
	if(value<lowConstraint)
		value=lowConstraint;
	return value;
}
float deadZone(float value,float deadzone)
{
	if(value>deadzone||value<-deadzone)
		return value;
	else
		return 0.0;
}
class Robot: public IterativeRobot
{
	LiveWindow *lw;

	CANJaguar franzRight;
	CANJaguar bachRight;
	CANJaguar bachLeft;
	CANJaguar franzLeft;
	RobotDrive myRobot;

	bool reverseDrive;
	bool reverseDrivePressed;
	float driveTurn;
	bool driveTurnPressed;

	CANJaguar spinz;
	Encoder spinzEnc;
	PIDController spinzController;
	float topTurn;
	int setShit;
	bool setShitPressed;
	bool usePID;

	CANJaguar liftz;
	float screwUp; //don't screwUp
	bool screwUpPressed;

	CANTalon tastyTotes;
	float tastySetVal;
	bool tastySetValPressed;

	//Encoder enc;
	Gyro gyro;
	//AnalogInput gyroAlso;

	Joystick throttle;
	Joystick stick;

	int fuckAll;

public:
	Robot() :
		franzRight(10),
		bachRight(11),
		bachLeft(12),
		franzLeft(13),
		myRobot(franzLeft, bachLeft, franzRight, bachRight),

		reverseDrive(false),
		reverseDrivePressed(false),
		driveTurn(0.0),
		driveTurnPressed(false),

		spinz(15),
		spinzEnc(0,1),
		spinzController(0.0,0.0,0.0,&spinzEnc,&spinz),
		topTurn(0.0),
		setShit(0),
		setShitPressed(false),
		usePID(false),

		liftz(14),
		screwUp(0.0),
		screwUpPressed(false),

		tastyTotes(17),
		tastySetVal(0.0),
		tastySetValPressed(false),

		//enc(0,1),
		gyro(0),
		//gyroAlso(0),

		throttle(0),
		stick(1),

		fuckAll(FUCK_EVERYTHING)
	{
		lw = LiveWindow::GetInstance();
	}
private:
	void RobotInit()
	{
		myRobot.SetSafetyEnabled(false);
		//gyro.SetDeadband(0.007);

		SmartDashboard::PutBoolean("useGyro",false);
		SmartDashboard::PutBoolean("resetGyro",false);
		SmartDashboard::PutNumber("tastyLift",0.0);
		SmartDashboard::PutBoolean("sendPidVal",false);
		SmartDashboard::PutBoolean("setPID",false);
		SmartDashboard::PutNumber("P",0.090);
		SmartDashboard::PutNumber("I",0.000);
		SmartDashboard::PutNumber("D",0.010);//90 deg values.
		//SmartDashboard::PutNumber("Setpoint",0.0);

	}

	void AutonomousInit()
	{

	}
	void AutonomousPeriodic()
	{

	}

	void TeleopInit()
	{
		spinzController.Enable();
	}
	void TeleopPeriodic()
	{
		/*SmartDashboard::PutNumber("Franz Right (FRC) drive",franzRight.Get());
		SmartDashboard::PutNumber( "Bach Right (BRO) drive",bachRight.Get());
		SmartDashboard::PutNumber( "Franz Left (FLY) drive",franzLeft.Get());
		SmartDashboard::PutNumber(  "Bach Left (BLT) drive",bachLeft.Get());*/
		if(usePID)
			spinzController.Enable();
		else
			spinzController.Disable();
		spinzController.SetSetpoint(setShit);//SmartDashboard::GetNumber("Setpoint"));
		SmartDashboard::PutNumber("setShit",setShit);
		/*if(SmartDashboard::GetBoolean("sendPidVal"))
			spinzController.Enable();
		else
		{

			spinz.Set(0);
			spinzController.Disable();
		}*/

		if(stick.GetRawButton(10))
		{
			driveTurn=(-throttle.GetRawAxis(2)+1.5)*0.4;
			usePID=true;
			//setShit=spinzEnc.Get();
		}
		else if(stick.GetRawButton(12))
		{
			driveTurn=-(-throttle.GetRawAxis(2)+1.5)*0.4;
			usePID=true;
			//setShit=spinzEnc.Get();
		}
		else
			driveTurn=0.0;

		driveTurn=constrain(driveTurn, 0.4, -0.4);
		if(stick.GetRawButton(2)&&!reverseDrivePressed)
		{
			reverseDrivePressed=true;
			reverseDrive=!reverseDrive;
		}
		else if(!stick.GetRawButton(2))
			reverseDrivePressed=false;

		if(!reverseDrive)
			myRobot.MecanumDrive_Cartesian(-stick.GetX(), -stick.GetY(), driveTurn,
				SmartDashboard::GetBoolean("useGyro") ? gyro.GetAngle() : 0.0
			);
		else
			myRobot.MecanumDrive_Cartesian(stick.GetX(), stick.GetY(), driveTurn,
				SmartDashboard::GetBoolean("useGyro") ? gyro.GetAngle() : 0.0
			);
		SmartDashboard::PutBoolean("reverse",reverseDrive);
		SmartDashboard::PutNumber("getEncPos",spinzEnc.Get());
		//SmartDashboard::PutNumber("enc",enc.Get());
		//SmartDashboard::PutNumber("encRate",enc.GetRate());
		spinzController.SetPID(SmartDashboard::GetNumber("P"),SmartDashboard::GetNumber("I")/100,SmartDashboard::GetNumber("D"));

		if(SmartDashboard::GetBoolean("resetGyro")) gyro.Reset();
		SmartDashboard::PutNumber("gyro",gyro.GetAngle());
		//SmartDashboard::PutNumber("gyroAlso",gyroAlso.GetVoltage());
		SmartDashboard::PutNumber("gyroRate 8 out of 8",gyro.GetRate());

		//SmartDashboard::PutNumber("controlGet",spinzController.Get());

		//spinz.Set(deadZone(throttle.GetRawAxis(4),0.05)/2);


		if(stick.GetRawButton(4)&&!tastySetValPressed)
		{
			tastySetValPressed=true;
			tastySetVal=0.20;
		}
		else if(stick.GetRawButton(4)&&tastySetValPressed)
		{
			tastySetVal+=0.02;
			tastySetVal=constrain(tastySetVal,0.7,0.0);
		}
		else if(stick.GetRawButton(1)&&!tastySetValPressed)
		{
			tastySetValPressed=true;
			tastySetVal=-0.15;
		}
		else if(stick.GetRawButton(1)&&tastySetValPressed)
		{
			tastySetVal-=0.02;
			tastySetVal=constrain(tastySetVal,0.0,-0.7);
		}
		else
		{
			tastySetValPressed=false;
			tastySetVal=0.0;
		}
		SmartDashboard::PutNumber("tastyLift",tastySetVal);
		tastyTotes.Set(tastySetVal);

		if(stick.GetRawButton(6))
		{
			usePID=false;
			topTurn=0.2;
			//setShit=spinzEnc.Get();
		}
		else if(stick.GetRawButton(8))
		{
			usePID=false;
			topTurn=-0.2
					;
			//setShit=spinzEnc.Get();
		}
		else if(stick.GetRawButton(14))
		{
			usePID=false;
			topTurn=0.5;
		}
		else if(stick.GetRawButton(16))
		{
			usePID=false;
			topTurn=-0.5;
		}
		else
		{
			topTurn=0.0;
		}

		if(stick.GetRawButton(7)&&!screwUpPressed)
		{
			screwUp=0.30;
			screwUpPressed=true;
		}
		else if(stick.GetRawButton(7)&&screwUpPressed)
		{
			screwUp+=0.01;
			screwUp=constrain(screwUp,0.5,0.0);
		}
		else if(stick.GetRawButton(5)&&!screwUpPressed)
		{
			screwUp=-0.3;
			screwUpPressed=true;
		}
		else if(stick.GetRawButton(5)&&screwUpPressed)
		{
			screwUp-=0.015;
			screwUp=constrain(screwUp,0.0,-1);
		}
		else if(stick.GetRawButton(15)&&!screwUpPressed)
		{
			screwUpPressed=true;
			screwUp=0.5;
		}
		else if(stick.GetRawButton(15)&&screwUpPressed)
		{
			screwUp+=0.02;
			screwUp=constrain(screwUp,0.8,0);
		}
		else if(stick.GetRawButton(13)&&!screwUpPressed)
		{
			screwUpPressed=true;
			screwUp=-0.4;
		}
		else if(stick.GetRawButton(13)&&screwUpPressed)
		{
			screwUp-=0.025;
			screwUp=constrain(screwUp,0.0,-1.0);
		}
		else
		{
			screwUpPressed=false;
			screwUp=0.0;
		}


		if(!usePID)
		{
			spinz.Set(topTurn);//throttle.GetRawAxis(4));
			setShit=spinzEnc.Get();
		}
		liftz.Set(screwUp);//deadZone(throttle.GetRawAxis(3),0.05));//controller.Get());//deadZone(throttle.GetRawAxis(3),0.05)/2);
		SmartDashboard::PutNumber("liftVal",liftz.Get());
		SmartDashboard::PutNumber("X",stick.GetX());
		SmartDashboard::PutNumber("Y",stick.GetY());
		/*SmartDashboard::PutNumber("spinz axis", throttle.GetRawAxis(4));
		SmartDashboard::PutNumber("liftz axis", throttle.GetRawAxis(3));
		SmartDashboard::PutNumber("spinz", spinz.Get());
		SmartDashboard::PutNumber("liftz", liftz.Get());*/


		SmartDashboard::PutBoolean("fucking buttons", throttle.GetRawButton(1));

		// Sage, DO NOT put a Wait() here
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
