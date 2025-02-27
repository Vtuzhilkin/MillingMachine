#pragma once
#include "main.h"
#include "math.h"
#include "StepperMotor.h"

struct Coordinates{
	Coordinates(int32_t x, int32_t y, int32_t z): coordinate_x(x), coordinate_y(y), coordinate_z(z){};
	int32_t coordinate_x;
	int32_t coordinate_y;
	int32_t coordinate_z;
};

class Machine{
	private:
		float velocity = 2.0;
		StepperMotor* MotorHorizontalX;
		StepperMotor* MotorHorizontalY;
		StepperMotor* MotorVerticalZ;
		
		Coordinates current_coordinates{0, 0, 0};
		Coordinates next_coordinates{0, 0, 0};
		
		bool openedCOMPort = false;
		bool readNextCoordinate = true;
		bool permissionStart = false;
		bool pauseProcessing = false;
		
		void formatedNumber(uint8_t* data, float number);
		
	public:
		Machine(StepperMotor* MotorX, StepperMotor* MotorY, StepperMotor* MotorZ): MotorHorizontalX(MotorX), MotorHorizontalY(MotorY), MotorVerticalZ(MotorZ) {};
		bool CheckStopMotors();
		void Start();
		void Stop();
		void Pause();
		void ReadNextCoordinate(uint8_t* data);
		void SetNextCoordinate();
		bool Calibrated();
		void SetVelocity(uint8_t* pVelocity);
		void SetCOMPort(bool open);
		bool GetCOMPort();
		void GetStatus(uint8_t* data);
		void SetPermissionStart(bool);
		bool GetPermissionStart();
		bool GetPauseProcessing();
		
};