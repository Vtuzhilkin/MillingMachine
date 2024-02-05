#pragma once
#include "main.h"
#include "math.h"
#include "StepperMotor.h"

struct Coordinate{
	Coordinate(float crd, int32_t num_st): coordinate(crd), number_steps(num_st){};
	float coordinate;
	int32_t number_steps;
	float part_velocity;
};

struct Coordinates{
	Coordinates(Coordinate x, Coordinate y, Coordinate z): coordinate_x(x), coordinate_y(y), coordinate_z(z){};
	Coordinate coordinate_x;
	Coordinate coordinate_y;
	Coordinate coordinate_z;
};

class Machine{
	private:
		StepperMotor* MotorHorizontalX;
		StepperMotor* MotorHorizontalY;
		StepperMotor* MotorVerticalZ;
		
		Coordinates current_coordinates{Coordinate{0.0,0}, Coordinate{0.0,0}, Coordinate{0.0,0}};
		Coordinates next_coordinates{Coordinate{0.0,0}, Coordinate{0.0,0}, Coordinate{0.0,0}};
		
		bool setNextCoordinate = false;
		bool readNextCoordinate = true;
		uint32_t i = 3;
		
	public:
		Machine(StepperMotor* MotorX, StepperMotor* MotorY, StepperMotor* MotorZ): MotorHorizontalX(MotorX), MotorHorizontalY(MotorY), MotorVerticalZ(MotorZ) {};
		void CalculateNumberStepsAndVelocity();
		void SetNextCoordinatesAndVelocity();
		bool CheckStopMotors();
		void Start();
		bool SetPermissionNextCoordinate();
		bool ReadPermissionNextCoordinate();
		void ReadNextCoordinate(uint8_t* data);
		
};