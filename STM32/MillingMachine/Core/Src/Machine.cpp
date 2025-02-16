#include "Machine.h"

void Machine::ReadNextCoordinate(uint8_t* data){
	float next_x = (float)((data[1] << 8) | data[0]) / 100.0f;
	float next_y = (float)((data[3] << 8) | data[2]) / 100.0f;
	float next_z = (float)((data[5] << 8) | data[4]) / 100.0f;
	
	int32_t number_steps_x = next_x*MotorHorizontalX->GetStepsOneMM();
	int32_t number_steps_y = next_y*MotorHorizontalY->GetStepsOneMM();
	int32_t number_steps_z = next_z*MotorVerticalZ->GetStepsOneMM();
	
	MotorHorizontalX->CalculateNumberSteps(number_steps_x);
	MotorHorizontalY->CalculateNumberSteps(number_steps_y);
	MotorVerticalZ->CalculateNumberSteps(number_steps_z);
	
	float coordinate_difference_x = number_steps_x - MotorHorizontalX->GetCurrentNumberSteps();
	float coordinate_difference_y = number_steps_y - MotorHorizontalY->GetCurrentNumberSteps();
	float coordinate_difference_z = number_steps_z - MotorVerticalZ->GetCurrentNumberSteps();
	
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
}

bool Machine::CheckStopMotors(){
	return !MotorHorizontalX->GetMoving() && !MotorHorizontalY->GetMoving() && !MotorVerticalZ->GetMoving();
}

void Machine::Start(){
	MotorHorizontalX->Start();
	MotorHorizontalY->Start();
	MotorVerticalZ->Start();
}

void Machine::Stop(){
	MotorHorizontalX->Stop();
	MotorHorizontalY->Stop();
	MotorVerticalZ->Stop();
}

bool Machine::Calibrated(){
	return MotorHorizontalX->Calibrated() && MotorHorizontalY->Calibrated() && MotorVerticalZ->Calibrated();
}

void Machine::SetVelocity(uint8_t* pVelocity){
	float new_velocity = (float)((pVelocity[0] << 8) | pVelocity[1]) / 100.0f;
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
	data[1] = 10;
	float x_coordinate = float(MotorHorizontalX->GetCurrentNumberSteps()) / MotorHorizontalX->GetStepsOneMM();
	formatedNumber(data + 2, x_coordinate);
	float y_coordinate = float(MotorHorizontalY->GetCurrentNumberSteps()) / MotorHorizontalY->GetStepsOneMM();
	formatedNumber(data + 4, y_coordinate);
	float z_coordinate = float(MotorVerticalZ->GetCurrentNumberSteps()) / MotorVerticalZ->GetStepsOneMM();
	formatedNumber(data + 6, z_coordinate);
	formatedNumber(data + 8, velocity);
	data[10] = Calibrated();
	data[11] = !CheckStopMotors();
}

void Machine::formatedNumber(uint8_t* data, float number){
	int16_t num_int = number*100;
	data[0] = (uint8_t)(num_int & 0xFF);
	data[1] = (uint8_t)((num_int >> 8) & 0xFF);
}

