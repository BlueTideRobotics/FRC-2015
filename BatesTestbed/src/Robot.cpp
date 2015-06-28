#include <cstdint>
#include "WPILib.h"
#include "PID.h"

#define FUCK_EVERYTHING 1


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
	Joystick stick;
	LiveWindow *lw;


	Servo servo;

	DigitalInput infra;
	AnalogInput potentiometer;

	//Encoder enc;
	CANJaguar jag1;
	CANJaguar jag2;
	//CANTalon tal1;

	//Compressor pcm; // Uncomment if you want to manually power the compressor or read pressure trigger.
	DoubleSolenoid piston;
	bool pistonVal;

	BuiltInAccelerometer accel;
	
	bool actuatePressed;
	int fuckAll;
public:
	Robot() :
		stick(0),
		lw(NULL),

		servo(9),

		infra(0),
		potentiometer(0),

		//enc(3,4),
		jag1(10),
		jag2(11),
		//tal1(b12),


		//pcm(),
		piston(4,3),
		pistonVal(true),
		accel(),

		actuatePressed(false),
		fuckAll(FUCK_EVERYTHING)
	{
	}

private:
	void RobotInit()
	{
		lw = LiveWindow::GetInstance();
	}

	void AutonomousInit()
	{
	}

	void AutonomousPeriodic()
	{
	}

	void TeleopInit()
	{

	}

	void TeleopPeriodic()
	{
		SmartDashboard::PutNumber("joystickX",stick.GetX());
		SmartDashboard::PutNumber("joystickY",stick.GetY());
		SmartDashboard::PutBoolean("fucking buttons", stick.GetRawButton(1));

		SmartDashboard::PutNumber("potentiometer voltage", potentiometer.GetVoltage());
		SmartDashboard::PutBoolean("infra",infra.Get());

		SmartDashboard::PutNumber("accelX",accel.GetX());
		SmartDashboard::PutNumber("accelY",accel.GetY());
		SmartDashboard::PutNumber("accelZ",accel.GetZ());

		servo.Set(
			trueMap(stick.GetX(), 1, -1, 1, 0) // trueMap allows use of entire joystick
		);
		SmartDashboard::PutNumber("servo", servo.Get());


		jag1.Set(0.0);
		jag2.Set(stick.GetY());
		//tal1.Set(stick.GetY());

		/*SmartDashboard::PutNumber("encpos", enc.Get());
		SmartDashboard::PutNumber("encspd", enc.GetRate());*/

		if (infra.Get() && !actuatePressed) {
			pistonVal=!pistonVal;
			piston.Set(pistonVal ? DoubleSolenoid::kForward : DoubleSolenoid::kReverse);

			actuatePressed = true;
		}
		else if (!infra.Get())
			actuatePressed = false;

		SmartDashboard::PutBoolean("piston forward", piston.Get() == DoubleSolenoid::kForward);
	}

	void TestPeriodic()
	{
	}
};

START_ROBOT_CLASS(Robot);
