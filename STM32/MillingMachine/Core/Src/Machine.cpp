#include "Machine.h"

void Machine::ReadNextCoordinate(uint8_t* data){
	float next_x = (data[2] & 0b10000000) ? -1 : 1;
	next_x *= (float)(((data[2] & 0b01111111) << 16) | (data[1] << 8) | data[0]) / 100.0f;

	float next_y = (data[5] & 0b10000000) ? -1 : 1;
	next_y *= (float)(((data[5] & 0b01111111) << 16) | (data[4] << 8) | data[3]) / 100.0f;

	float next_z = (data[8] & 0b10000000) ? -1 : 1;
	next_z *= (float)(((data[8] & 0b01111111) << 16) | (data[7] << 8) | data[6]) / 100.0f;
	
	next_coordinates.coordinate_x = next_x*MotorHorizontalX->GetStepsOneMM();
	next_coordinates.coordinate_y = next_y*MotorHorizontalY->GetStepsOneMM();
	next_coordinates.coordinate_z = next_z*MotorVerticalZ->GetStepsOneMM();
	
	readNextCoordinate = false;
}


void Machine::SetNextCoordinate(){
	MotorHorizontalX->CalculateNumberSteps(next_coordinates.coordinate_x);
	MotorHorizontalY->CalculateNumberSteps(next_coordinates.coordinate_y);
	MotorVerticalZ->CalculateNumberSteps(next_coordinates.coordinate_z);
	
	float coordinate_difference_x = next_coordinates.coordinate_x - MotorHorizontalX->GetCurrentNumberSteps();
	float coordinate_difference_y = next_coordinates.coordinate_y - MotorHorizontalY->GetCurrentNumberSteps();
	float coordinate_difference_z = next_coordinates.coordinate_z - MotorVerticalZ->GetCurrentNumberSteps();
	
	float hypotenuse = sqrt(coordinate_difference_x*coordinate_difference_x + coordinate_difference_y*coordinate_difference_y);
	
	float part_velocity_x = 1;
	float part_velocity_y = 1;
	float part_velocity_z = 1;
	
	if(hypotenuse != 0){
		part_velocity_x = abs(coordinate_difference_x/hypotenuse);
		if(part_velocity_x < 0){
			part_velocity_x = 1;
		}
		part_velocity_y = abs(coordinate_difference_y/hypotenuse);
		if(part_velocity_z < 0){
			part_velocity_y = 1;
		}
	}
	MotorHorizontalX->ChangeVelocity(part_velocity_x);
	MotorHorizontalY->ChangeVelocity(part_velocity_y);
	
	readNextCoordinate = true;
}

bool Machine::CheckStopMotors(){
	return !MotorHorizontalX->GetMoving() && !MotorHorizontalY->GetMoving() && !MotorVerticalZ->GetMoving();
}

void Machine::Start(){
	MotorHorizontalX->Start();
	MotorHorizontalY->Start();
	MotorVerticalZ->Start();
	permissionStart = false;
}

void Machine::Stop(){
	MotorHorizontalX->Stop();
	MotorHorizontalY->Stop();
	MotorVerticalZ->Stop();
	readNextCoordinate = true;
	permissionStart = false;
}


void Machine::Pause(){
	MotorHorizontalX->Stop();
	MotorHorizontalY->Stop();
	MotorVerticalZ->Stop();
}

bool Machine::Calibrated(){
	return MotorHorizontalX->Calibrated() && MotorHorizontalY->Calibrated() && MotorVerticalZ->Calibrated();
}

void Machine::SetVelocity(uint8_t* pVelocity){
	float new_velocity = (float)((pVelocity[1] << 8) | pVelocity[0]) / 100.0f;
	MotorHorizontalX->SetVelocity(new_velocity);
	MotorHorizontalY->SetVelocity(new_velocity);
	MotorVerticalZ->SetVelocity(new_velocity);
}

void Machine::SetCOMPort(bool open){
	openedCOMPort = open;
}

bool Machine::GetCOMPort(){
	return openedCOMPort;
}

void Machine::GetStatus(uint8_t* data){
	data[0] = 'P';
	data[1] = 12;
	float x_coordinate = float(MotorHorizontalX->GetCurrentNumberSteps()) / MotorHorizontalX->GetStepsOneMM();
	formatedNumber(data + 2, x_coordinate);
	float y_coordinate = float(MotorHorizontalY->GetCurrentNumberSteps()) / MotorHorizontalY->GetStepsOneMM();
	formatedNumber(data + 5, y_coordinate);
	float z_coordinate = float(MotorVerticalZ->GetCurrentNumberSteps()) / MotorVerticalZ->GetStepsOneMM();
	formatedNumber(data + 8, z_coordinate);
	formatedNumber(data + 11, velocity);
	data[13] = Calibrated();
	data[13] |= (!CheckStopMotors() << 1);
	data[13] |= (readNextCoordinate << 2);
}

void Machine::formatedNumber(uint8_t* data, float number){
	uint32_t num_int = number * 100;
	data[0] = (uint8_t)(num_int & 0xFF);
	data[1] = (uint8_t)((num_int >> 8) & 0xFF);
	if (number < 0) {
		data[2] = ((uint8_t)((num_int >> 16) & 0xFF)) | 0b10000000;
	}
	else {
		data[2] = (uint8_t)((num_int >> 16) & 0xFF);
	}
}


void Machine::SetPermissionStart(bool permission){
	permissionStart = permission;
}

bool Machine::GetPermissionStart(){
	return permissionStart;
}

