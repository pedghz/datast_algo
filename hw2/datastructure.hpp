// Definition of Datastructure class, hw2 of TIE-20100/TIE-20106

#ifndef DATASTRUCTURE_HPP
#define DATASTRUCTURE_HPP

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <utility>
using std::pair;

#include <unordered_map>
using std::unordered_map;

#include <map>
using std::map;

#include <algorithm>

using namespace std;

using PersonID = string;
PersonID const NO_ID = "";

using Salary = int;
Salary const NO_SALARY = -1;


struct Person
{
    PersonID my_id;
    string name;
    string title;
    Salary salary;
    PersonID boss_id;
    Person* boss_pointer;
    vector<PersonID> underlings_vec_id;
};

class Datastructure
{
public:
    Datastructure();
    ~Datastructure();

    void add_person(string name, PersonID id, string title, Salary salary);
    void remove_person(PersonID id);

    string get_name(PersonID id);
    string get_title(PersonID id);
    Salary get_salary(PersonID id);
    vector<PersonID> find_persons(string name);
    vector<PersonID> personnel_with_title(string title);

    void change_name(PersonID id, string new_name);
    void change_salary(PersonID id, Salary new_salary);

    void add_boss(PersonID id, PersonID bossid);

    unsigned int size();
    void clear();

    vector<PersonID> underlings(PersonID id);
    vector<PersonID> personnel_alphabetically();
    vector<PersonID> personnel_salary_order();

    PersonID find_ceo();
    PersonID nearest_common_boss(PersonID id1, PersonID id2);
    pair<unsigned int, unsigned int> higher_lower_ranks(PersonID id);

    PersonID min_salary();
    PersonID max_salary();

    PersonID median_salary();
    PersonID first_quartile_salary();
    PersonID third_quartile_salary();

private:
    unordered_map<PersonID, Person*>  people_unordered_map;
    multimap<string, PersonID> map_name_sorted;
    multimap<Salary, PersonID> map_salary_sorted;
    vector<Person*> people_pointer_vec;
};

#endif // DATASTRUCTURE_HPP
