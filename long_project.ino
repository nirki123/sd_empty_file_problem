#include <U8x8lib.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <SD.h>


const int joy_y = A0;
const int joy_x = A1;
const uint8_t joy_click = 2;
const uint8_t lcd_cs = 3;
const uint8_t sd_cs = 4;

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

class Point {
public:
	int x;
	int y;

	uint8_t scroller;

	Point(int x, int y) {
		this->x = x;
		this->y = y;
	}

	//Point();
};

class Static_functions {
public:
	static uint16_t maximize(uint16_t number, uint16_t max) {
		if (number > max) {
			number = max;
		}
		return number;
	}

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

	static void manage_cs(boolean lcd) {
		if (lcd) {
			digitalWrite(lcd_cs, LOW);
			digitalWrite(sd_cs, HIGH);
		}
		else {
			digitalWrite(lcd_cs, HIGH);
			digitalWrite(sd_cs, LOW);
		}
	}
};

class Joy_stick {
public:
	const uint8_t threshold = 128;
	boolean released = false;

	boolean selected(boolean release) {
		if (!digitalRead(joy_click)) {
			if (release) {
				if (released) {
					released = false;
					return true;
				}
			}
			else {
				return true;
			}
		}
		else {
			released = true;
			return false;
		}

	}

	Point read() {
		Point point = Point(analogRead(joy_x) - 512, analogRead(joy_y) - 512);
		return point;
	}

	uint8_t which_direction(Point point) {
		if (point.x > threshold && point.y > threshold) {
			return 1;
		}
		else if (point.x > threshold && point.y < -threshold) {
			return 2;
		}
		else if (point.x < -threshold && point.y < -threshold) {
			return 3;
		}
		else if (point.x < -threshold && point.y > threshold) {
			return 4;
		}
		else {
			return 0;
		}
	}

	int update_y(int number, boolean negative, boolean direction, uint16_t millis) {
		Point point = read();
		if (direction) {
			if (point.y >= threshold) {
				number++;
				delay(millis);
			}
			else if (point.y <= -threshold) {
				number--;
				delay(millis);
			}
		}

		else {
			if (point.y >= threshold) {
				number--;
				delay(millis);
			}
			else if (point.y <= -threshold) {
				number++;
				delay(millis);
			}
		}

		if (number < 0 && !negative) {
			number = 0;
		}
		return number;
	}

	int update_x(int number, boolean negative) {
		Point point = read();
		if (point.x >= threshold) {
			number++;
			delay(200);
		}
		else if (point.x <= -threshold) {
			number--;
			delay(200);
		}

		if (number < 0 && !negative) {
			number = 0;
		}
		return number;
	}
};

Joy_stick joy_stick;

class Function {
public:
	string name;
	unsigned long int* parameter = new unsigned long int[3];
	int loop_counter = 0;
	unsigned long int time;
	boolean null = false;
	uint8_t name_scroller = 0;
	uint8_t my_place_in_array;
	uint8_t time_scroller;

	string to_capital(string letter) {
		letter = string(letter.value() - 32);
		return letter;
	}

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


	void print() {
		Serial.println("name: ");
		Serial.print(name.value());
		if (strcmp(name.value(), "vibrato") == 0) {
			Serial.print(" speed: ");
			Serial.print(parameter[0]);
			Serial.print(" degrees: ");
			Serial.print(parameter[1]);
			Serial.print(" times: ");
			Serial.print(parameter[2]);
			Serial.println("");
		}
		else if (strcmp(this->name.value(), "rotate") == 0) {
			Serial.print(" speed: ");
			Serial.print(parameter[0]);
			Serial.print(" direction: ");
			Serial.print(parameter[1]);
			Serial.print(" degrees: ");
			Serial.print(parameter[2]);
			Serial.println("");
		}
		else if (strcmp(this->name.value(), "delay") == 0) {
			Serial.print(" seconds: ");
			Serial.print(parameter[0]);
			Serial.println("");
		}
	}

	boolean loop_condition() {
		if (strcmp(this->name.value(), "loop") == 0) {
			Serial.println("in loop condition");
			Serial.println(loop_counter);
			Serial.println(parameter[0]);
			if (loop_counter < parameter[0]) {
				loop_counter++;
				return true;
			}
			else {
				return false;
			}
		}
		else if (strcmp(this->name.value(), "time") == 0) {
			Serial.println(parameter[1]);
			return int((millis() - time) / parameter[1]) < parameter[0];
		}
	}

	char* show_function() {
		string letter = string(to_capital(name).value());
		string zero1 = "";
		string zero2 = "";
		string zero3 = "";
		string output;

		if (parameter[0] < 100) {
			strcat(zero1.characters, "0");
		}
		if (parameter[0] < 10) {
			strcat(zero1.characters, "0");
		}
		if (parameter[1] < 100) {
			strcat(zero2.characters, "0");
		}
		if (parameter[1] < 10) {
			strcat(zero2.characters, "0");
		}
		if (parameter[2] < 100) {
			strcat(zero3.characters, "0");
		}
		if (parameter[2] < 10) {
			strcat(zero3.characters, "0");
		}

		parameter[0] = Static_functions::maximize(parameter[0], 999);
		if (strcmp(this->name.value(), "time") != 0) {
			parameter[1] = Static_functions::maximize(parameter[1], 999);
		}
		parameter[2] = Static_functions::maximize(parameter[2], 999);

		if (strcmp(this->name.value(), "vibrato") == 0 || strcmp(this->name.value(), "rotate") == 0) {
			strcat(output.characters, letter.value());
			strcat(output.characters, "(");
			strcat(output.characters, zero1.value());
			strcat(output.characters, parameter[0] + "");
			strcat(output.characters, zero2.value());
			strcat(output.characters, parameter[1] + "");
			strcat(output.characters, zero3.value());
			strcat(output.characters, parameter[2] + "");
			strcat(output.characters, ")");

			return output.value();
		}
		else if (strcmp(this->name.value(), "loop") == 0 || strcmp(this->name.value(), "delay") == 0) {
			strcat(output.characters, letter.value());
			strcat(output.characters, "(");
			strcat(output.characters, zero1.value());
			strcat(output.characters, parameter[0] + "");
			strcat(output.characters, ")");

			return output.value();
		}
		else if (strcmp(this->name.value(), "time") == 0) {
			string format;
			switch (parameter[1]) {
			case 1000: format = string("sec"); break;
			case 60000: format = string("min"); break;
			case 3600000: format = string("hou"); break;
			}

			strcat(output.characters, letter.value());
			strcat(output.characters, "(");
			strcat(output.characters, format.value());
			strcat(output.characters, ",");
			strcat(output.characters, zero1.value());
			strcat(output.characters, parameter[0] + "");
			strcat(output.characters, ")");

			return output.value();
		}
	}

	void scroll_name(boolean first_function) {
		if (!first_function) {
			if (strcmp(this->name.value(), "vibrato") == 0) {
				name_scroller = 0;
			}
			else if (strcmp(this->name.value(), "rotate") == 0) {
				name_scroller = 1;
			}
			else if (strcmp(this->name.value(), "delay") == 0) {
				name_scroller = 2;
				parameter[1] = 0;
				parameter[2] = 0;
			}

			name_scroller = joy_stick.update_y(name_scroller, false, true, 300);
			if (name_scroller > 2) {
				name_scroller = 2;
			}

			switch (name_scroller) {
			case 0: strcat(name.characters, "vibrato"); break;
			case 1: strcat(name.characters, "rotate"); break;
			case 2: strcat(name.characters, "delay"); break;
			}
		}
		else {
			if (strcmp(this->name.value(), "loop") == 0) {
				name_scroller = 0;
				parameter[1] = 1000;
				time_scroller = 0;
			}
			else if (strcmp(this->name.value(), "time") == 0) {
				name_scroller = 1;
			}

			name_scroller = joy_stick.update_y(name_scroller, false, true, 0);
			if (name_scroller > 1) {
				name_scroller = 1;
			}

			if (name_scroller == 0) {
				name = string("loop");
			}
			else {
				name = string("time");
			}
		}
	}

	void scroll_parameter(uint8_t which_parameter) {
		if (strcmp(this->name.value(), "time") == 0) {
			which_parameter = !which_parameter;
		}
		if (strcmp(this->name.value(), "time") == 0 && which_parameter == 1) {
			switch (parameter[1]) {
			case 1000: time_scroller = 0; break;
			case 60000: time_scroller = 1; break;
			case 3600000: time_scroller = 2; break;
			}

			time_scroller = joy_stick.update_y(time_scroller, false, false, 300);
			time_scroller = Static_functions::maximize(time_scroller, 2);

			switch (time_scroller) {
			case 0: parameter[1] = 1000; break;
			case 1: parameter[1] = 60000; break;
			case 2: parameter[1] = 3600000; break;
			}
		}
		else {
			if (joy_stick.selected(false)) {
				parameter[which_parameter] = joy_stick.update_y(parameter[which_parameter], false, false, 0);
				parameter[which_parameter] = joy_stick.update_y(parameter[which_parameter], false, false, 0);
			}
			else {
				parameter[which_parameter] = joy_stick.update_y(parameter[which_parameter], false, false, 100);
			}

			if (parameter[which_parameter] > 999) {
				parameter[which_parameter] = 999;
			}
		}
	}
};

boolean is_number(char character) {
	//Serial.println(character >= 48 && character <= 58);
	return character >= 48 && character <= 58;
}
class Data {
public:
	int amount_of_actions = 0;

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
			Serial.println("2");

			Serial.println(file_name);
			Serial.println("here7");
			File file = SD.open(file_name);
			Serial.println("here6");
			string parameter = string("0");
			unsigned long int* parameters = new unsigned long int[3];
			char character;
			Serial.println("here3");
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
		//Static_functions::manage_cs(true);
	}



	Function* delete_an_action(Function* functions, int action, int actions_amount) {
		while (action < actions_amount) {
			functions[action].create_parameters(functions[action + 1].name, functions[action + 1].parameter);
			action++;
		}
		return functions;
	}

	Function* add_an_action(Function* functions, Function function, int action, int actions_amount) {
		for (int counter = actions_amount; counter > action; counter--) {
			functions[counter].create_parameters(functions[counter - 1].name, functions[counter - 1].parameter);
		}
		functions[action].create_parameters(function.name, function.parameter);
		return functions;
	}

	Function* replace_an_action(Function* functions, Function function, int action) {
		functions[action].create_parameters(function.name, function.parameter);
		return functions;
	}
};


class Draw {
public:
	void draw_strings_with_numbers(uint8_t x, uint8_t y, string output, boolean first_function) {
		if (first_function) {
			lcd.setFont(u8g2_font_pxplusibmcga_8r);
		}
		else {
			lcd.setFont(u8g2_font_pxplusibmcgathin_8r);
		}
		for (int counter = 0; counter < output.length(); counter++) {
			lcd.drawGlyph(counter * 8 + x, y, output.value()[counter]);
		}

	}
};

string s = string("aaa");
Draw draw;
Data data;
Function* functions = new Function[100];
uint8_t actions_amount;

void setup() {
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
}

void loop() {
	//lcd.clearBuffer();
	//Serial.println(s.value());
	//draw.draw_strings_with_numbers(0, 50, "a", false);
	//Serial.println("here");
	//lcd.setFont(u8g2_font_pxplusibmcgathin_8f);

	//Serial.println("here4");
	for (uint8_t counter = 0; counter < 0; counter++) {
		//strcat(s.characters, functions[counter].show_function());
	}
	//Serial.println(s.value());
	//draw.draw_strings_with_numbers(20, 20, s.value(), false);

	//lcd.sendBuffer();
}
