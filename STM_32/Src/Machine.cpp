#include "Machine.h"

void Machine::ReadNextCoordinate(uint8_t* data){
	float x = (float)(data[1]-48)*100 + (float)(data[2]-48)*10 + (float)(data[3]-48) + (float)(data[4]-48)/10 + (float)(data[5]-48)/100/* + (float)(data[6]-48)/1000 + (float)(data[7]-48)/10000*/;
	if(data[0] == 45) x = -x;
	float y = (float)(data[7]-48)*100 + (float)(data[8]-48)*10 + (float)(data[9]-48) + (float)(data[10]-48)/10 + (float)(data[11]-48)/100/* + (float)(data[14]-48)/1000 + (float)(data[15]-48)/10000*/;
	if(data[6] == 45) y = -y;
	float z = (float)(data[13]-48)*100 + (float)(data[14]-48)*10 + (float)(data[15]-48) + (float)(data[16]-48)/10 + (float)(data[17]-48)/100/* + (float)(data[22]-48)/1000 + (float)(data[23]-48)/10000*/;
	if(data[12] == 45) z = -z;
	next_coordinates.coordinate_x.coordinate = x;
	next_coordinates.coordinate_y.coordinate = y;
	next_coordinates.coordinate_z.coordinate = z;
	readNextCoordinate = false;
	setNextCoordinate = true;
}

void Machine::CalculateNumberStepsAndVelocity(){
	next_coordinates.coordinate_x.number_steps = next_coordinates.coordinate_x.coordinate*MotorHorizontalX->GetStepsOneMM();
	next_coordinates.coordinate_y.number_steps = next_coordinates.coordinate_y.coordinate*MotorHorizontalY->GetStepsOneMM();
	next_coordinates.coordinate_z.number_steps = next_coordinates.coordinate_z.coordinate*MotorVerticalZ->GetStepsOneMM();
	
	float coordinate_difference_x = next_coordinates.coordinate_x.coordinate - current_coordinates.coordinate_x.coordinate;
	float coordinate_difference_y = next_coordinates.coordinate_y.coordinate - current_coordinates.coordinate_y.coordinate;
	//float coordinate_difference_z = next_coordinates.coordinate_z.coordinate - current_coordinates.coordinate_z.coordinate;
	float hypotenuse = sqrt(coordinate_difference_x*coordinate_difference_x + coordinate_difference_y*coordinate_difference_y);
	if(hypotenuse != 0){
		next_coordinates.coordinate_x.part_velocity = abs(coordinate_difference_x/hypotenuse);
		if(next_coordinates.coordinate_x.part_velocity < 0){
			next_coordinates.coordinate_x.part_velocity = 1;
		}
		next_coordinates.coordinate_y.part_velocity = abs(coordinate_difference_y/hypotenuse);
		if(next_coordinates.coordinate_y.part_velocity < 0){
			next_coordinates.coordinate_y.part_velocity = 1;
		}
		//next_coordinates.coordinate_z.part_velocity = abs(coordinate_difference_z/hypotenuse);
	}else{
		next_coordinates.coordinate_x.part_velocity = 1;
		next_coordinates.coordinate_y.part_velocity = 1;
		next_coordinates.coordinate_z.part_velocity = 1;
	}
}

void Machine::SetNextCoordinatesAndVelocity(){
	MotorHorizontalX->CalculateNumberSteps(current_coordinates.coordinate_x.number_steps, next_coordinates.coordinate_x.number_steps);
	MotorHorizontalX->ChangeVelocity(next_coordinates.coordinate_x.part_velocity);
	MotorHorizontalY->CalculateNumberSteps(current_coordinates.coordinate_y.number_steps, next_coordinates.coordinate_y.number_steps);
	MotorHorizontalY->ChangeVelocity(next_coordinates.coordinate_y.part_velocity);
	MotorVerticalZ->CalculateNumberSteps(current_coordinates.coordinate_z.number_steps, next_coordinates.coordinate_z.number_steps);
	//MotorVerticalZ->ChangeVelocity(next_coordinates.coordinate_z.part_velocity);
	current_coordinates = next_coordinates;
	readNextCoordinate = true;
	setNextCoordinate = false;
	i++;
}

bool Machine::CheckStopMotors(){
	return !MotorHorizontalX->GetMoving() && !MotorHorizontalY->GetMoving() && !MotorVerticalZ->GetMoving();
}

void Machine::Start(){
	MotorHorizontalX->Start();
	MotorHorizontalY->Start();
	MotorVerticalZ->Start();
}

bool Machine::SetPermissionNextCoordinate(){
	return setNextCoordinate;
}

bool Machine::ReadPermissionNextCoordinate(){
	return readNextCoordinate;
}





