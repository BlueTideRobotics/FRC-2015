#include <cstdint>
#include <math.h>

#include "WPILib.h"
#include "PID.h"
#include "SageFunctions.h"

float toExponentialScale(float joystickVal) {
	if (fabs(joystickVal) <= 0.05) // built in deadzone
		return 0.0;
	int sign = (joystickVal > 0) ? 1 : -1;
	float value = pow(16, fabs(joystickVal) - 1);
	return sign * value;
}

class Robot: public IterativeRobot
{
	LiveWindow *lw;

	CANJaguar frontRightDrive;
	CANJaguar rearRightDrive;
	CANJaguar rearLeftDrive;
	CANJaguar frontLeftDrive;
	RobotDrive myRobot;

	bool reverseDrive;
	bool reverseDrivePressed;

	float driveTurn;
	bool driveTurnPressed;
	float driveX;
	float driveY;

	CANJaguar spinz;
	Encoder spinzEnc;
	float topTurn;

	CANJaguar liftz;
	float screwUp; //don't screwUp
	bool screwUpPressed;

	CANTalon toteTilt;
	float toteTiltSetVal;
	bool toteTiltPressed;

	CANTalon toteGrab;
	float toteGrabSetVal;

	CANTalon toteIntakeWheels;
	float toteIntakeSetVal;

	//Encoder enc;
	Gyro gyro;

	Joystick throttle;
	Joystick stick;

public:
	Robot() :
		frontRightDrive(10),
		rearRightDrive(11),
		rearLeftDrive(12),
		frontLeftDrive(13),
		myRobot(frontLeftDrive, rearLeftDrive, frontRightDrive, rearRightDrive),

		reverseDrive(false),
		reverseDrivePressed(false),
		driveTurn(0.0),
		driveTurnPressed(false),
		driveX(0.0),
		driveY(0.0),

		spinz(15),
		spinzEnc(0,1),
		topTurn(0.0),

		liftz(14),
		screwUp(0.0),
		screwUpPressed(false),

		toteTilt(16),
		toteTiltSetVal(0.0),
		toteTiltPressed(false),

		toteGrab(17),
		toteGrabSetVal(0.0),

		toteIntakeWheels(18),
		toteIntakeSetVal(0.0),

		//enc(0,1),
		gyro(0),

		throttle(0),
		stick(1)
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

		SmartDashboard::PutNumber("* driveTurn init", 0.1);
		SmartDashboard::PutNumber("* driveTurn accel", 0.02);

		SmartDashboard::PutNumber("* toteTilt init", 0.2);
		SmartDashboard::PutNumber("* toteTilt accel", 0.02);

		SmartDashboard::PutNumber("* toteGrab value", 0.2);
		SmartDashboard::PutNumber("* toteIntake value", 0.5);

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
		// Drive Reverse
		if (stick.GetRawButton(2) && !reverseDrivePressed) {
			reverseDrive = !reverseDrive;
			reverseDrivePressed = true;
		}
		else if (!stick.GetRawButton(2)) {
			reverseDrivePressed = false;
		}

		// Drive turn
		if (stick.GetRawButton(14) && !driveTurnPressed) {
			driveTurn = SmartDashboard::GetNumber("* driveTurn init");
			driveTurnPressed = true;
		}
		else if (stick.GetRawButton(16) && !driveTurnPressed) {
			driveTurn = -SmartDashboard::GetNumber("* driveTurn init");
			driveTurnPressed = true;
		}
		if (stick.GetRawButton(14) && driveTurnPressed) {
			driveTurn += SmartDashboard::GetNumber("* driveTurn accel");
		}
		else if (stick.GetRawButton(16) && driveTurnPressed) {
			driveTurn -= SmartDashboard::GetNumber("* driveTurn accel");
		}
		else {
			driveTurn = 0.0;
			driveTurnPressed = false;
		}
		driveTurn = constrain(driveTurn, 1.0, -1.0);

		if (stick.GetRawButton(4)) { // operate swivel/lift with XY
			topTurn = toExponentialScale(stick.GetX());
			screwUp = toExponentialScale(stick.GetY());

			driveX = 0.0;
			driveY = 0.0;
		}
		else { // operate Mecanum drive with XY
			topTurn = 0.0;
			screwUp = 0.0;

			driveX = reverseDrive ? -stick.GetX() : stick.GetX();
			driveY = reverseDrive ? -stick.GetY() : stick.GetY();
		}


		// Tote Tilt (aka Tasty Totes)
		if(stick.GetRawButton(7)&&!toteTiltPressed)
		{
			toteTiltPressed=true;
			toteTiltSetVal = SmartDashboard::GetNumber("* toteTilt init");
		}
		else if(stick.GetRawButton(7)&&toteTiltPressed)
		{
			toteTiltSetVal += SmartDashboard::GetNumber("* toteTilt accel");
		}
		else if(stick.GetRawButton(5)&&!toteTiltPressed)
		{
			toteTiltPressed = true;
			toteTiltSetVal = -SmartDashboard::GetNumber("* toteTilt init");
		}
		else if(stick.GetRawButton(5)&&toteTiltPressed)
		{
			toteTiltSetVal -= SmartDashboard::GetNumber("* toteTilt accel");
		}
		else
		{
			toteTiltPressed=false;
			toteTiltSetVal=0.0;
		}
		toteTiltSetVal = constrain(toteTiltSetVal,1.0,-1.0);


		// Grabber
		if (stick.GetRawButton(11)) { // grabber
			toteGrabSetVal = SmartDashboard::GetNumber("* toteGrab value");
		}
		else if (stick.GetRawButton(9)) { // grabber
			toteGrabSetVal = -SmartDashboard::GetNumber("* toteGrab value");
		}
		else {
			toteGrabSetVal = 0.0;
		}

		// Tote Intake Wheels
		if (stick.GetRawButton(1))
			toteIntakeSetVal = SmartDashboard::GetNumber("* toteIntake value");
		else
			toteIntakeSetVal = 0.0;

		// variables -> motor settings
		myRobot.MecanumDrive_Cartesian(driveX, driveY, driveTurn, SmartDashboard::GetBoolean("useGyro") ? gyro.GetAngle() : 0.0);
		spinz.Set(topTurn);
		liftz.Set(screwUp);

		toteGrab.Set(toteGrabSetVal);
		toteTilt.Set(toteTiltSetVal);
		toteIntakeWheels.Set(toteIntakeSetVal);

		// Tell SmartDashboard what we did.
		SmartDashboard::PutNumber("Drive X", driveX);
		SmartDashboard::PutNumber("Drive Y", driveY);
		SmartDashboard::PutNumber("Drive turn", driveTurn);
		SmartDashboard::PutNumber("Spinz set", topTurn);
		SmartDashboard::PutNumber("Liftz set", screwUp);

		SmartDashboard::PutNumber("Tote Grab set", toteGrabSetVal);
		SmartDashboard::PutNumber("Tote Tilt set", toteTiltSetVal);
		SmartDashboard::PutNumber("Tote intake wheel set", toteIntakeSetVal);

		// Gyro things
		SmartDashboard::PutNumber("gyro", gyro.GetAngle());
		if (SmartDashboard::GetBoolean("resetGyro"))
			gyro.Reset();

		// Sage, DO NOT put a Wait() here
	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
