#include <ctime>
#include "utils.hpp"

class Logger {
public:
	Logger(const Logger &obj) = delete;

	static Logger *getInstance(string file_name){
		if (l_instance == NULL){
			l_instance = new Logger(file_name);

			return l_instance;
		}
		else{
			return l_instance;
		}
	}

	void log(string log_text){
		time_t now = time(0);
		tm *curr = localtime(&now);
		cout << curr->tm_hour << endl;

		fprintf(fptr, "%d:%d:%d-> %s\n", curr->tm_hour, curr->tm_min, curr->tm_sec, log_text.c_str());
	}

private:
	FILE *fptr;
	Logger(){}
	Logger(string file_name){
		openFile(&fptr, file_name, "w");
	}
	static Logger *l_instance;
};
