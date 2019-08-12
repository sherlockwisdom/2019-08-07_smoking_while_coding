/*
 *Date: 2019-08-2019
 *Description: JSON parser for CPP
 *Author: Sherlock
 */
#include <iostream>
#include <set>
#include <map>
#include <vector>
using namespace std;


class Json{
	string* write_head;
	string name_buffer="",value_buffer="", standard_name;

	map<string,string>name_value_buffer;
	
	vector<Json>name_object_buffer;

	enum write_state{NAME,VALUE};
	write_state WRITE_STATE=NAME;
	public:
		Json(){}
		Json(string standard_name):standard_name(standard_name){}
		void write_head_switch(), new_data(), write_data(char), add_child(Json), stat(int);
		string get_name_buffer() { return this->name_buffer; }
};


void Json::write_head_switch(){
	if(this->WRITE_STATE==NAME) this->WRITE_STATE=VALUE;
	else if(this->WRITE_STATE==VALUE) this->WRITE_STATE=NAME;
	//cout<<"WRITE_STATE_AFTER_SWITCH: " <<this->WRITE_STATE<<endl;
}

void Json::new_data() {
	if(this->name_buffer.empty()) {
		//cout<<"WRITE_STATE="<<this->WRITE_STATE<<endl;
		return;
	}
	//cout<<"--making new data: "<<this->name_buffer<<"|"<<this->value_buffer<<endl;
	this->name_value_buffer.insert(make_pair(this->name_buffer,this->value_buffer));
	this->name_buffer="";
	this->value_buffer="";
	this->WRITE_STATE=NAME;
}

void Json::write_data(char _char){
	if(this->WRITE_STATE==NAME) this->name_buffer+=(char)_char;
	else if(this->WRITE_STATE==VALUE) this->value_buffer+=(char)_char;
}

void Json::add_child(Json json_object){
	this->name_object_buffer.push_back(json_object);
}


void Json::stat(int tab_index=0){
	auto tabs=[=](){
		string ret_value="";
		for(int i=0;i<tab_index;++i) ret_value.push_back('\t');
		return ret_value;
	};
	cout<<tabs()<<this->standard_name<<"--->[name_value_buffer_size: "<<this->name_value_buffer.size()<<"]"<<endl;
	for(auto name_value:this->name_value_buffer){
		cout<<tabs()<<name_value.first<<"="<<name_value.second<<endl;
	}
	cout<<tabs()<<this->standard_name<<"--->[name_object_buffer_size: "<<this->name_object_buffer.size()<<"]"<<endl;
	if(!this->name_object_buffer.empty()) {
		short int tab_index=1;
		for(auto object:this->name_object_buffer){
			object.stat(++tab_index);
		}
	}
	else cout << endl;
}


auto extract_objects(string sample_string) {
	vector<int>object_start_pnt, object_end_pnt;
	multimap<int,map<int,int>>container;
	vector<Json>previous_last_one_stack;
	for(int i=0;i<sample_string.size();++i){
		//static vector<int> previous_last_one_stack;
		char _char=sample_string[i];
		//cout<<"["<<_char<<": "<<i<<"]"<<endl;
		switch(_char){
			case '{':{
					 if(!previous_last_one_stack.empty()) {
						 //cout << "name_buffer: " << previous_last_one_stack.back().get_name_buffer() << endl;
						 Json json(previous_last_one_stack.back().get_name_buffer());
						 previous_last_one_stack.back().new_data();
						 previous_last_one_stack.push_back(json);
					 }
					 else {
						 Json json;
						 previous_last_one_stack.push_back(json);
					 }
					 //previous_last_one_stack.push_back(i);
					 break;
				 }
			case '}':{
					//the last one takes it
					//shit would hit the fan if empty and .back() called
					/*map<int,int>new_object{{previous_last_one_stack.back(),i}};
					container.insert(make_pair(previous_last_one_stack.at(previous_last_one_stack.size()>1?previous_last_one_stack.size()-2:previous_last_one_stack.size()-1), new_object));
					previous_last_one_stack.pop_back();*/

					 if(previous_last_one_stack.size() > 1){
						 previous_last_one_stack.at(previous_last_one_stack.size() -2).add_child(previous_last_one_stack.back());
						 previous_last_one_stack.pop_back();
					 }
					 else
						 previous_last_one_stack.at(previous_last_one_stack.size() -1);

					break;
				 }
			case ':':
				 previous_last_one_stack.back().write_head_switch();
				 break;
			case ',':
				 previous_last_one_stack.back().new_data();
				 break;
			default:
				 previous_last_one_stack.back().write_data(_char);
				 break;
		}
	}
	return previous_last_one_stack;
}



int main(){
	string sample_string="{name:sherlock,object:{new_object:{},new_object_2:{}}}";
	//string sample_json="{\"nam:{e\" :\"sherlock wisdom\",\"name2\":\"sherlock holmes\",new_object:{\"new_name\":\"new_sherlock_wisdom\"}}";

	auto objects = extract_objects(sample_string);

	//closest proximity in linear line
	/*for(auto parent_object:objects){
		cout<<parent_object.first<<"| ";
		for(auto child_object:parent_object.second)
			cout<<child_object.first<<"="<<child_object.second<<endl;
	}*/

	for(auto object:objects){
		object.stat();
	}

	return 0;
}
