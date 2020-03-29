//====================================================================================================
// Name        : schedule_ga.cpp
// Author      : Radoslaw Pudelko
// Description : Optimization of the university department course schedule using the genetic algorithm
//====================================================================================================
#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <ctime>
#include <iomanip>
#include "schedule_ga.hpp"

using namespace std;

int randNum(int min, int max){
	return min + ( rand() % ( max - min + 1 ) );
}

vector<vector<int>> getCourses(ifstream& _file){

	string line;
	string class_start = "<<NEW_CLASS_IDENTIFIER>>";
	string class_end = "<<END_CLASS_IDENTIFIER>>";
	string class_id_string = "<CLASS_ID>";
	string prof_id_string = "<PROF_ID>";
	string group_id_string = "<GROUP_ID>";
	string room_id_string = "<ROOM_ID>";
	string class_duration_string = "<CLASS_DURATION>";
	vector<int> course;
	vector<vector<int>> all_courses;
	while(getline(_file,line))
	    {
	      if(line.find(class_start)!=string::npos){
	    	  course = {};
	      }
	      else if(line.find(class_end)!=string::npos){
	    	  all_courses.push_back(course);
	      }
	      else if(line.find(class_id_string)!=string::npos){
	    	  string class_id(line,class_id_string.length(),line.length()-class_id_string.length());
	    	  course.push_back(stoi(class_id));
	      }
	      else if(line.find(group_id_string)!=string::npos){
	    	  string group_id(line,group_id_string.length(),line.length()-group_id_string.length());
	    	  course.push_back(stoi(group_id));
	      }
	      else if(line.find(prof_id_string)!=string::npos){
	    	  string prof_id(line,prof_id_string.length(),line.length()-prof_id_string.length());
	    	  course.push_back(stoi(prof_id));
	      }
	      else if(line.find(room_id_string)!=string::npos){
	    	  string room_id(line,room_id_string.length(),line.length()-room_id_string.length());
	    	  course.push_back(stoi(room_id));
	      }
	      else if(line.find(class_duration_string)!=string::npos){
	    	  string class_duration(line,class_duration_string.length(),line.length()-class_duration_string.length());
	    	  course.push_back(stoi(class_duration));
	      }
	    }
	return all_courses;
}

vector<vector<vector<int>>> getPopulation(vector<vector<int>> _courses){

	random_device rd; // obtain a random number from hardware
	mt19937 eng(rd()); // seed the generator
	uniform_int_distribution<> distrDAY(MIN_DAY, MAX_DAY); // define the range
	uniform_int_distribution<> distrHOUR(MIN_HOUR, MAX_HOUR); // define the range

	vector<int> lab;
	vector<vector<int>> schedule;
	vector<vector<vector<int>>> population;
	for(size_t j=0;j<POPULATION_SIZE;j++){
		for(size_t i=0;i<_courses.size();i++){
			lab = _courses.at(i);
			lab.push_back(distrDAY(eng));
			lab.push_back(distrHOUR(eng));
			schedule.push_back(lab);
		}
	population.push_back(schedule);
	schedule={};
	}
	return population;
}

/*
int countConflicts(vector<vector<int>> _schedule, int type){
	int i,j;
	int n=0;
	vector<int> temp;
	for (i=0;i<15;i++){
		temp = _schedule.at(i);
		for (j=0;j<15;j++){
			if(_schedule[j][type]==temp[type] && _schedule[j][4]==temp[4] && _schedule[j][5]==temp[5] && j!=i){
				n++;
			}
		}
	}
	return n/2;
}
*/

int countConflictHours(vector<vector<int>> _schedule, int type){
	unsigned int i,j;
	int n=0;
	vector<int> temp;
	for (i=0;i<_schedule.size();i++){
		temp = _schedule.at(i);
		for (j=0;j<_schedule.size();j++){
			if(_schedule[j][type]==temp[type] && _schedule[j][5]==temp[5] && j!=i){ // sprawdzam czy to ten sam dzien
				if((_schedule[j][6]+_schedule[j][4])<=(temp[6]+temp[4]) && (_schedule[j][6]+_schedule[j][4])>temp[6]){ // jesli godziny sie pokrywaja
					n+= temp[6]+temp[4]-_schedule[j][6]; // licze ile godzin sie pokrywa
				}
			}
			else if(_schedule[j][type]!=temp[type] && _schedule[j][5]==temp[5] && j!=i && type==1 && min(_schedule[j][type],temp[type])%100==0 && max(_schedule[j][type],temp[type])/min(_schedule[j][type],temp[type])==1){ // jesli to wyklad to musze zbadac wiecej kolizji
				if((_schedule[j][6]+_schedule[j][4])<=(temp[6]+temp[4]) && (_schedule[j][6]+_schedule[j][4])>temp[6]){
					n+= temp[6]+temp[4]-_schedule[j][6]; // licze ile godzin sie pokrywa
				}
			}
		}
	}
	return n/2;
}

vector<vector<int>> getProfPref(ifstream& _file){
	string line;
	string prof_start = "<<NEW_PROF_IDENTIFIER>>";
	string prof_end = "<<END>>";
	string id = "<ID>";
	string h1 = "<H1>";
	string h2 = "<H2>";
	string day = "<DAY>";
	int _id,_h1,_h2,_day;
	vector<int> single_preference;
	vector<vector<int>> all_preferences;
	while(getline(_file,line))
		{
		  if(line.find(id)!=string::npos){
			  string prof_id(line,id.length(),line.length()-id.length());
			  _id = stoi(prof_id);
		  }
		  else if(line.find(day)!=string::npos){
			  string prof_day(line,day.length(),line.length()-day.length());
			  _day = stoi(prof_day);
			  single_preference.push_back(_id);
			  single_preference.push_back(_day);
		  }
		  else if(line.find(h1)!=string::npos){
			  string prof_h1(line,h1.length(),line.length()-h1.length());
			  _h1 = stoi(prof_h1);
			  single_preference.push_back(_h1);
		  }
		  else if(line.find(h2)!=string::npos){
			  string prof_h2(line,h2.length(),line.length()-h2.length());
			  _h2 = stoi(prof_h2);
			  single_preference.push_back(_h2);
			  all_preferences.push_back(single_preference);
			  single_preference = {};
		  }
		}
	return all_preferences;
}



int checkProfPref(vector<vector<int>> _schedule, vector<vector<int>> _prof_pref){
	unsigned int i,j;
	int n=0;
	vector<int> temp;
	for (i=0;i<_schedule.size();i++){
		for(j=0;j<_prof_pref.size();j++){
			if(_schedule[i][2]==_prof_pref[j][0] && _schedule[i][5]==_prof_pref[j][1]){
				if((_schedule[i][6]<_prof_pref[j][2] && _prof_pref[j][3]<_schedule[i][6]+_schedule[i][4]) ||
						(_prof_pref[j][2]<_schedule[i][6] && _schedule[i][6]+_schedule[i][4]<_prof_pref[j][3]) ||
						(_schedule[i][6]<_prof_pref[j][3] && _prof_pref[j][3]<_schedule[i][6]+_schedule[i][4]) ||
						(_schedule[i][6]<_prof_pref[j][2] && _prof_pref[j][2]<_schedule[i][6]+_schedule[i][4])){
					n++;
				}
			}
		}
	}
	return n;
}

int countIntervalHours(vector<vector<int>> _schedule, int type){


	int temp=1;
	for(size_t i=0;i<_schedule.size();i++){
		if(_schedule[i][type]>temp){
			temp = _schedule[i][1];
		}
	}
	int dur,min,max;
	int interval_time=0;

	//cout << temp << endl;
	for(int n=1;n<MAX_DAY+1;n++){ // dla kazdego dnia
		for(int j=1;j<temp+1;j++){ // dla kazdej klasy
			dur=0;
			min=MAX_HOUR;
			max=MIN_HOUR;
			for(unsigned i=0;i<_schedule.size();i++){ // przechodze po zajeciach i patrze co wyjdzie
				if(_schedule[i][type]==j && _schedule[i][5]==n){
					dur+=_schedule[i][4];
					if(_schedule[i][6]<min){
						min = _schedule[i][6];
					}
					if(_schedule[i][6]+_schedule[i][4]>max){
						max = _schedule[i][6]+_schedule[i][4];
					}
				}
			}
			if(max>dur){
				interval_time = interval_time + max - dur;
			}

			//cout << "day=" << n << " group/prof=" << j << endl;
			//cout << "min=" << min << " max=" << max << " dur=" << dur << endl;
		}
	}

	//cout << "ILOSC OKIENEK=" << interval_time << endl;
	return interval_time;
}




vector<int> bestFitness(vector<vector<vector<int>>>& _population, vector<vector<int>> _prof_pref){

    int group=1;
    int professor=2;
    int room=3;

    vector<int> fitness;
	vector<vector<int>> schedule;
	int value, interv;
	int c_p=0;
	int c_r=0;
	int c_g=0;
	int p_p=0;
	int i_p=0;
	int i_g=0;
	for(int n=0;n<POPULATION_SIZE;n++){
		schedule = _population.at(n);
		c_p = countConflictHours(schedule,professor);
		c_r = countConflictHours(schedule,room);
		c_g = countConflictHours(schedule,group);
		p_p = checkProfPref(schedule,_prof_pref);
		i_g = countIntervalHours(schedule,group);
		i_p = countIntervalHours(schedule,professor);

		value = 10*c_p + 10*c_g + 10*c_r + p_p;
		interv = i_g + i_p;

		cout << "PROF=" << 10*c_p << " GROUPS=" << 10*c_g << " ROOMS=" << 10*c_r << " PROF_PREF=" << p_p << " INTER_GR=" << i_g << " INTER_PR=" << i_p ;
		cout <<	" || FITNESS=" << value << " INTERV=" << interv << endl;
		fitness.push_back(value);
	}

	int min, second_min;
	int min_index, second_min_index;

	if(fitness.at(0)<fitness.at(1)){
		min = fitness.at(0);
		min_index = 0;
		second_min = fitness.at(1);
		second_min_index = 1;
	} else {
		min = fitness.at(1);
		min_index = 1;
		second_min = fitness.at(0);
		second_min_index = 0;
	}


	for (size_t i=2;i<fitness.size();i++){
		if (fitness[i]<min){
			min = fitness[i];
			min_index = i;
		}
		else if(fitness[i]<second_min && fitness[i]>=min){
			second_min = fitness[i];
			second_min_index = i;
		}
	}
	fitness.clear();
	schedule.clear();

    cout << "MIN FITNESS DLA PLANU NR " << min_index << " I WYNOSI: " << min << endl;
    cout << "2 MIN FITNESS DLA PLANU NR " << second_min_index << " I WYNOSI: " << second_min << endl;

	return {min,min_index,second_min,second_min_index,interv};
}

vector<vector<int>> mutateSchedule(vector<vector<int>>& _schedule){

	vector<vector<int>> new_schedule = _schedule;
	int to_mutate, new_day, new_hour;
	for(int i=0;i<MUTATION_NUMBER;i++){
		to_mutate = randNum(0,_schedule.size()-1);
		new_day = randNum(MIN_DAY,MAX_DAY);
		new_hour = randNum(MIN_HOUR,MAX_HOUR);
		new_schedule[to_mutate][5] = new_day;
		new_schedule[to_mutate][6] = new_hour;
	}
	return new_schedule;
}

vector<vector<vector<int>>> mutatePopulation(vector<vector<vector<int>>>& _population,vector<vector<int>> _prof_pref){
	vector<vector<vector<int>>> new_population;
	vector<int> fitness = bestFitness(_population,_prof_pref);
	vector<vector<int>> schedule = _population.at(fitness.at(1));
	vector<vector<int>> new_schedule;
	new_population.push_back(schedule);
	for(size_t i=0;i<_population.size()-1;i++){
		new_schedule = mutateSchedule(schedule);
		new_population.push_back(new_schedule);
	}
	cout << "new pop size: " << new_population.size() << endl;
	return new_population;
}

vector<vector<int>> crossoverSchedules(vector<vector<int>>& _schedule1, vector<vector<int>>& _schedule2){

	vector<vector<int>> new_schedule, crossover_schedule;
	if(randNum(0,1)){
		new_schedule = _schedule1;
		crossover_schedule = _schedule2;
	} else {
		new_schedule = _schedule2;
		crossover_schedule = _schedule1;
	}

	unsigned int crossover_point = randNum(1,new_schedule.size()-2);
	for(size_t i=0;i<new_schedule.size();i++){
		if(i>=crossover_point){
			new_schedule.at(i) = crossover_schedule.at(i);
		}
	}

	return new_schedule;

}

vector<vector<vector<int>>> crossoverPopulation(vector<vector<vector<int>>>& _population,vector<vector<int>> _prof_pref){
	vector<vector<vector<int>>> new_population;
	vector<int> fitness = bestFitness(_population,_prof_pref);
	vector<vector<int>> schedule1 = _population.at(fitness.at(1));
	vector<vector<int>> schedule2 = _population.at(fitness.at(3));
	vector<vector<int>> new_schedule;
	new_population.push_back(schedule1);
	new_population.push_back(schedule2);
	for(size_t i=0;i<_population.size()-2;i++){
		new_schedule = crossoverSchedules(schedule1,schedule2);
		new_population.push_back(new_schedule);
	}
	cout << "new pop size: " << new_population.size() << endl;
	return new_population;
}



int main() {

	vector<vector<int>> courses;

	ifstream file_in("data_in.data");
	if (file_in.is_open()){
		courses = getCourses(file_in);
		file_in.close();
	} else {
		cout << "ERROR" << endl;
		return 0;
	}

	vector<vector<int>> prof_preferences;

	ifstream prof_data("prof_data.data");
	if (prof_data.is_open()){
		prof_preferences = getProfPref(prof_data);
		prof_data.close();
	} else {
		cout << "ERROR" << endl;
		return 0;
	}


	ofstream file_out("data_out.data");
	if (file_out.is_open())
	{
		//file_out << "ITERATION, BEST_FITNESS\n";
		file_out.close();
	} else {
		cout << "ERROR" << endl;
		return 0;
	}


    ofstream outfile;
    outfile.open("data_out.data", ios_base::app);

	vector<vector<vector<int>>> population = getPopulation(courses);
	vector<int> fit;
	vector<vector<vector<int>>> new_population;

	time_t tstart, tend;
	tstart = time(0);
	int cnt=0;
    do {
		new_population = mutatePopulation(population,prof_preferences);
		fit = bestFitness(new_population,prof_preferences);
		population = new_population;
		if(fit.at(0)!=0 && randNum(0,100)<=CROSSOVER_PROB){
			new_population = crossoverPopulation(population,prof_preferences);
			fit = bestFitness(new_population,prof_preferences);
			population = new_population;
			cout << "WITH CROSSOVER" << endl;
		}
		cnt++;
		cout << "ITERATIONS: " << cnt << endl;
		outfile << cnt << ", " << fit.at(0) << "\n";
    } while (fit.at(0)!=0);

    int count=0;
    for ( const auto &schedule : population ){
 	   cout << "SCHEDULE " << count << endl;
       for ( const auto &row : schedule ) {
    	   for( const auto &s : row ){
    		cout << setw(6) << s << " ";
    	   }
    		cout << endl;
       }
       count++;
       cout << endl;
    }

    cout << "ITERATION: " << cnt << endl;
    tend = time(0);
    cout << "It took "<< difftime(tend, tstart) <<" second(s)."<< endl;
    return 0;


}


