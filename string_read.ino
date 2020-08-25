#include <U8x8lib.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <SD.h>

U8G2_ST7920_128X64_F_6800 lcd(U8G2_R0, 5, 6, 7, 8, 9, 10, 11, 12, 23, U8X8_PIN_NONE, 22);

class string {
public:
	char characters[30];
	string(char characters[30]) {
		for (uint8_t counter = 0; counter < 30; counter++) {
			this->characters[counter] = NULL;
		}
		strcpy(this->characters, characters);
	}

	string() {
	}

	uint8_t length() {
		uint8_t output = 0;
		for (uint8_t counter = 0; counter < 30; counter++) {
			if (characters[counter] != NULL) {
				output++;
			}
		}
		return output;
	}

	char* value() {
		return characters;
	}
};

class Static_functions {
public:
	static int to_int(string number) {
		int len = number.length();
		int current_number;
		int out = 0;
		for (uint8_t counter = 0; counter < len; counter++) {
			current_number = int(number.value()[counter]) - 48;
			out += round(current_number * pow(10, (len - 1) - counter));
		}
		return out;
	}
};

class Function {
public:
	string name;
	unsigned long int* parameter = new unsigned long int[3];
	void create_parameters(string name, unsigned long int* parameter_) {
		strcat(this->name.characters, name.value());

		if (strcmp(this->name.value(), "vibrato") == 0) {
			for (uint8_t counter = 0; counter < 3; counter++) {
				parameter[counter] = parameter_[counter];
			}
		}
		else if (strcmp(this->name.value(), "rotate") == 0) {
			for (uint8_t counter = 0; counter < 3; counter++) {
				parameter[counter] = parameter_[counter];
			}
		}
		else if (strcmp(this->name.value(), "delay") == 0) {
			parameter[0] = parameter_[0];
		}
		else if (strcmp(this->name.value(), "loop") == 0) {
			parameter[0] = parameter_[0];
		}
		else if (strcmp(this->name.value(), "time") == 0) {
			parameter[0] = parameter_[0];
			parameter[1] = parameter_[1];
		}
		else if (strcmp(this->name.value(), "error") == 0) {
			//fix it
		}
		else {
			//another error
		}
	}
};

class Data {
public:
	boolean is_number(char letter) {
		return int(letter) >= 48 && int(letter) <= 57;
	}

	uint8_t actions_counter(string file) {
		char* file_name = file.value();
		uint8_t counter = 0;
		if (SD.exists(file_name)) {
			Serial.println("here");
			File file = SD.open(file_name);
			while (file.available()) {
				if (int(file.read()) == int(';')) {
					counter++;
				}
			}
		}
		return counter;
	}
	Function* read_txt_file(string file) {
		//Static_functions::manage_cs(false);
		char* file_name = file.value();
		Function* function = new Function[100];
		delay(500);
		if (SD.exists(file_name)) {
			Serial.println(file_name);
			string parameter = string("0");
			unsigned long int* parameters = new unsigned long int[3];
			char character;
			File file = SD.open(file_name);
			Serial.println(file.size());
			delay(500);
			if (file.available()) {
				Serial.println("here5");
				for (int counter = 0; file.available(); counter++) {
					character = file.read();
					Serial.println(character);

					if (character == int('v')) {
						for (int counter_ = 0; counter_ < 3; counter_++) {
							parameter = "0";
							character = file.read();
							while (is_number(character)) {
								strcat(parameter.characters, character + "");
								character = file.read();
							}
							parameters[counter_] = Static_functions::to_int(parameter);
						}
						function[counter].create_parameters("vibrato", parameters);
					}
					else if (character == int('r')) {
						for (int counter_ = 0; counter_ < 3; counter_++) {
							parameter = "0";
							character = file.read();
							while (is_number(character)) {
								strcat(parameter.characters, character + "");
								character = file.read();
							}
							parameters[counter_] = Static_functions::to_int(parameter);
						}
						function[counter].create_parameters("rotate", parameters);
					}
					else if (character == int('d')) {
						parameter = "0";
						character = file.read();
						while (is_number(character)) {
							strcat(parameter.characters, character + "");
							character = file.read();
						}
						parameters[0] = Static_functions::to_int(parameter);
						function[counter].create_parameters("delay", parameters);
					}
					else if (character == int('l')) {
						parameter = "0";
						character = file.read();
						while (is_number(character)) {
							strcat(parameter.characters, character + "");
							character = file.read();
						}
						parameters[0] = Static_functions::to_int(parameter);
						Serial.println(parameters[0]);
						function[0].create_parameters("loop", parameters);
						if (counter != 0) {
							counter--;
						}
					}
					else if (character = int('t')) {
						character = file.read();
						if (character == 's') {
							parameters[1] = 1000;
						}
						else if (character == 'm') {
							parameters[1] = 60000;
						}
						else if (character == 'h') {
							parameters[1] = 3600000;
						}
						parameter = "0";
						character = file.read();
						while (is_number(character)) {
							strcat(parameter.characters, character + "");
							character = file.read();
						}
						parameters[0] = Static_functions::to_int(parameter);
						function[0].create_parameters("time", parameters);
						if (counter != 0) {
							counter--;
						}
					}
					else {
						function[counter].name = "error";
					}
				}
				return function;
			}
			else {
				Serial.println("file is empty");
				Serial.println("");
			}
		}
		else {
			Serial.println("doesn't exist");
		}

		return function;
	}
};

Data data;
uint8_t actions_amount;
Function* function;

void setup(){
	Serial.begin(115200);
	while (!Serial);
	pinMode(53, OUTPUT);
	if (!SD.begin(4)) {
		Serial.println("initialization failed!");
		while (true);
	}
	else {
		Serial.println("initialization succeed!");
	}

	actions_amount = data.actions_counter("r1.txt");
	Serial.println(actions_amount);
	//function = data.read_txt_file("r1.txt");

	/*for (uint8_t counter = 0; counter < actions_amount; counter++) {
		Serial.println(function[counter].name.value());
	}*/
}

void loop(){


}
