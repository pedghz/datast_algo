// Definition of Datastructure class, hw2 of TIE-20100/TIE-20106

#include "datastructure.hpp"

// For debug
#include <iostream>
using std::cerr;
using std::endl;

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = rand() % range;
    return static_cast<Type>(start+num);
}

bool nth_func (Person* p1, Person* p2) {return p1->salary < p2->salary;}

// O(m) in which 'm' is number of direct or undirect bosses of each person.
vector<PersonID> bosses_id_list(vector<PersonID>& bosses_ids, Person* ptr)
{
    if (ptr->boss_pointer != nullptr)
    {
        bosses_ids.push_back(ptr->boss_pointer->my_id);
        bosses_id_list(bosses_ids, ptr->boss_pointer);
    }
    return bosses_ids;
}

// O(m*k) in which 'm' is number of direct or undirect bosses of each person and 'k' is the size of boss vector first person.
PersonID find_common_boss(vector<PersonID>& bosses_ids, Person* ptr)
{
    PersonID common_boss = NO_ID;
    if (ptr->boss_pointer != nullptr)
    {
        for (vector<PersonID>::iterator iter2 = bosses_ids.begin(); iter2 != bosses_ids.end(); ++iter2)
            if ( *iter2 == ptr->boss_pointer->my_id ) {
                common_boss = *iter2;
                return common_boss;
            }
        common_boss = find_common_boss(bosses_ids, ptr->boss_pointer);
    }
    return common_boss;
}

// O(m)  in which 'm' is number of direct or undirect bosses of each person.
unsigned int depth(Person* ptr, unsigned int tmp_depth)
{
    if (ptr->boss_pointer != nullptr)
    {
        tmp_depth ++;
        tmp_depth = depth(ptr->boss_pointer, tmp_depth);
    }
    return tmp_depth;
}

Datastructure::Datastructure()
{
}

Datastructure::~Datastructure()
{
    for (Person* person : people_pointer_vec)
    {
        delete person;
    }
    people_unordered_map.clear();
    map_name_sorted.clear();
    map_salary_sorted.clear();
    people_pointer_vec.clear();
}

// O(n)
void Datastructure::add_person(string name, PersonID id, string title, Salary salary)
{
    Person* tmp_person = new Person;
    tmp_person->name = name;
    tmp_person->salary = salary;
    tmp_person->title = title;
    tmp_person->my_id = id;
    tmp_person->boss_id = NO_ID;
    tmp_person->boss_pointer = nullptr;
    tmp_person->underlings_vec_id = {};
    if (people_unordered_map.find(id) == people_unordered_map.end())
    {
        people_unordered_map.insert({id,tmp_person});
        map_name_sorted.insert({name,id});
        map_salary_sorted.insert({salary,id});
        people_pointer_vec.push_back(tmp_person);
    }
}

// O(n)
void Datastructure::remove_person(PersonID id)
{
    unordered_map<PersonID, Person*>::iterator iter_to_removing_person_unordered_map = people_unordered_map.find(id);

    if (iter_to_removing_person_unordered_map != people_unordered_map.end())
    {
        vector<Person*>::iterator iter_to_removing_person_vector = find(people_pointer_vec.begin(), people_pointer_vec.end(), iter_to_removing_person_unordered_map->second);
        string name = (iter_to_removing_person_unordered_map->second)->name;
        Salary salary = (iter_to_removing_person_unordered_map->second)->salary;
        Person* newBoss = (iter_to_removing_person_unordered_map->second)->boss_pointer;
        vector<PersonID> tmp_vec_underlings_id = (iter_to_removing_person_unordered_map->second)->underlings_vec_id;

        if (newBoss != nullptr)
        {
            (*newBoss).underlings_vec_id.erase(find((*newBoss).underlings_vec_id.begin(), (*newBoss).underlings_vec_id.end(), id));
            for (vector<PersonID>::iterator iter = tmp_vec_underlings_id.begin(); iter != tmp_vec_underlings_id.end(); ++iter)
            {
                (*newBoss).underlings_vec_id.push_back(*iter);
            }
        }
        for (unordered_map<PersonID, Person*>::iterator iter = people_unordered_map.begin(); iter != people_unordered_map.end(); ++iter)
            if ((iter->second)->boss_id == id)
            {
                (iter->second)->boss_id = (iter_to_removing_person_unordered_map->second)->boss_id;
                (iter->second)->boss_pointer = newBoss;
            }

        people_unordered_map.erase(iter_to_removing_person_unordered_map);
        people_pointer_vec.erase(iter_to_removing_person_vector);

        pair <multimap<string,PersonID>::iterator, multimap<string,PersonID>::iterator> name_range_iter;
        name_range_iter = map_name_sorted.equal_range(name);
        for (std::multimap<string,PersonID>::iterator it=name_range_iter.first; it!=name_range_iter.second; ++it)
            if (it->first == name && it->second == id)
            {
                map_name_sorted.erase(it);
                break;
            }

        pair <multimap<Salary,PersonID>::iterator, multimap<Salary,PersonID>::iterator> salary_range_iter;
        salary_range_iter = map_salary_sorted.equal_range(salary);
        for (std::multimap<Salary,PersonID>::iterator it = salary_range_iter.first; it != salary_range_iter.second; ++it)
            if (it->first == salary && it->second == id)
            {
                map_salary_sorted.erase(it);
                break;
            }
    }
}

// O(n) - Theta(1)
string Datastructure::get_name(PersonID id)
{
    string name = people_unordered_map.at(id)->name;
    if ( name != NO_ID ) {
        return name;
    }
    return NO_ID;
}

// O(n)
string Datastructure::get_title(PersonID id)
{
    string title = people_unordered_map.at(id)->title;
    if ( title != NO_ID ) {
        return title;
    }
    return NO_ID;
}

// O(n)
Salary Datastructure::get_salary(PersonID id)
{
    int salary = people_unordered_map.at(id)->salary;
    if ( salary != NO_SALARY ) {
        return salary;
    }
    return NO_SALARY;
}

// O(log(n))
vector<PersonID> Datastructure::find_persons(string name)
{
    vector<PersonID> employee_ids;
    pair<multimap<string, PersonID>::iterator, multimap<string, PersonID>::iterator> name_range_iter = map_name_sorted.equal_range(name);
    for (multimap<string,PersonID>::iterator it = name_range_iter.first; it != name_range_iter.second; ++it)
        employee_ids.push_back(it->second);
    sort(employee_ids.begin(), employee_ids.end());
    return employee_ids;
}

// O(n)
vector<PersonID> Datastructure::personnel_with_title(string title)
{
    vector<PersonID> employee_ids;
    for (unordered_map<PersonID, Person*>::iterator iter = people_unordered_map.begin(); iter != people_unordered_map.end(); ++iter)
        if ( (*((*iter).second)).title == title ) {
            employee_ids.push_back((*iter).first);
        }
    sort(employee_ids.begin(), employee_ids.end());
    return employee_ids;
}

// O(n)
void Datastructure::change_name(PersonID id, string new_name)
{
    unordered_map<PersonID, Person*>::iterator iter_to_changing_person = people_unordered_map.find(id);
    if (iter_to_changing_person!= people_unordered_map.end())
    {
    Person* changing_person = iter_to_changing_person->second;
    string old_name = changing_person->name;
    changing_person->name = new_name;
    pair <multimap<string,PersonID>::iterator, multimap<string,PersonID>::iterator> name_range_iter;
    name_range_iter = map_name_sorted.equal_range(old_name);
    for (std::multimap<string,PersonID>::iterator it=name_range_iter.first; it!=name_range_iter.second; ++it)
        if (it->second == id)
        {
            map_name_sorted.erase(it);
            map_name_sorted.insert({new_name, id});
            break;
        }
    }
}

// O(n)
void Datastructure::change_salary(PersonID id, Salary new_salary)
{
    unordered_map<PersonID, Person*>::iterator iter_to_changing_person = people_unordered_map.find(id);
    if (iter_to_changing_person!= people_unordered_map.end())
    {
        Person* changing_person = iter_to_changing_person->second;
        Salary old_salary = changing_person->salary;
        changing_person->salary = new_salary;
        pair <multimap<Salary,PersonID>::iterator, multimap<Salary,PersonID>::iterator> salary_range_iter;
        salary_range_iter = map_salary_sorted.equal_range(old_salary);
        for (std::multimap<Salary,PersonID>::iterator it=salary_range_iter.first; it!=salary_range_iter.second; ++it)
            if (it->second == id)
            {
                map_salary_sorted.erase(it);
                map_salary_sorted.insert({new_salary, id});
                break;
            }
    }
}

// O(n)
void Datastructure::add_boss(PersonID id, PersonID bossid)
{
    unordered_map<PersonID, Person*>::iterator employee_iter = people_unordered_map.find(id);
    unordered_map<PersonID, Person*>::iterator boss_iter = people_unordered_map.find(bossid);
    PersonID employee_id = (*employee_iter).first;
    Person* boss_ptr = (*boss_iter).second;

    if (employee_iter == people_unordered_map.end())
    {
        add_person("",id,"",0);
    }
    (*(employee_iter->second)).boss_id = bossid;

    if (boss_iter == people_unordered_map.end())
    {
        add_person("",bossid,"",0);
    }
    (*((employee_iter->second))).boss_pointer = boss_ptr;
    ((*(boss_iter->second)).underlings_vec_id).push_back(employee_id);
}

// Constant complexity
unsigned int Datastructure::size()
{
    return people_unordered_map.size();
}

// O(n)
void Datastructure::clear()
{  
    for (Person* person : people_pointer_vec)
    {
        delete person;
    }
    people_unordered_map.clear();
    map_name_sorted.clear();
    map_salary_sorted.clear();
    people_pointer_vec.clear();
}

// max[O(n), O(m*log(m))]
vector<PersonID> Datastructure::underlings(PersonID id)
{
    Person* tmp_person = people_unordered_map.at(id);
    sort(tmp_person->underlings_vec_id.begin(), tmp_person->underlings_vec_id.end());
    return (tmp_person)->underlings_vec_id;
}

// O(n)
vector<PersonID> Datastructure::personnel_alphabetically()
{
    vector<PersonID> employee_ids;
    for (multimap<string,PersonID>::iterator iter = map_name_sorted.begin(); iter != map_name_sorted.end(); ++iter)
        employee_ids.push_back((*iter).second);
    return employee_ids;
}

// O(n)
vector<PersonID> Datastructure::personnel_salary_order()
{
    vector<PersonID> employee_ids;
    for (multimap<Salary,PersonID>::iterator iter = map_salary_sorted.begin(); iter != map_salary_sorted.end(); ++iter)
        employee_ids.push_back((*iter).second);
    return employee_ids;
}

// O(n)
PersonID Datastructure::find_ceo()
{
    int found_ceo = 0;
    PersonID ceo = NO_ID;

    for (pair<PersonID, Person*> employee : people_unordered_map) {
        if (employee.second->boss_pointer == nullptr || employee.second->boss_id == NO_ID)
        {
            found_ceo++;
            ceo = employee.first;
        }
    }

    if (found_ceo == 1)
        return ceo;
    return NO_ID;
}

// O(m*k) in which 'm' is number of direct or undirect bosses of each person and 'k' is the size of boss vector first person.
PersonID Datastructure::nearest_common_boss(PersonID id1, PersonID id2)
{
    vector<PersonID> bosses_list_for_id1 = {};
    PersonID common_boss = NO_ID;
    bosses_list_for_id1 = bosses_id_list(bosses_list_for_id1, people_unordered_map.at(id1));
    common_boss = find_common_boss(bosses_list_for_id1, people_unordered_map.at(id2));
    return common_boss;
}

// O(n*m) in which 'n' is number of all employees and 'm' is number of direct or undirect bosses of each person.
pair<unsigned int, unsigned int> Datastructure::higher_lower_ranks(PersonID id)
{
    unsigned int high = 0;
    unsigned int low = 0 ;
    unsigned int tmp_person_depth = 0;
    unsigned int person_depth = depth(people_unordered_map.at(id), 0);

    for (unordered_map<PersonID, Person*>::iterator iter = people_unordered_map.begin(); iter != people_unordered_map.end() ; ++iter)
    {
        tmp_person_depth = depth(iter->second, 0);
        if (tmp_person_depth < person_depth)
            high++;
        if (tmp_person_depth > person_depth)
            low++;
    }
    return {high, low};
}

// Constant complexity
PersonID Datastructure::min_salary()
{
    if (map_salary_sorted.size() > 0) {
        multimap<Salary, PersonID>::iterator iter = map_salary_sorted.begin();
        return (*iter).second;
    }
    return NO_ID;
}

// Constant complexity
PersonID Datastructure::max_salary()
{
    if (people_pointer_vec.size() > 0) {
        multimap<Salary, PersonID>::reverse_iterator iter = map_salary_sorted.rbegin();
        return (*iter).second;
    }
    return NO_ID;
}

// O(log(n))
PersonID Datastructure::median_salary()
{
    if (people_pointer_vec.size() > 0) {
        nth_element (people_pointer_vec.begin(), people_pointer_vec.begin() + people_pointer_vec.size()/2 , people_pointer_vec.end(), nth_func);
        return people_pointer_vec.at(people_pointer_vec.size()/2)->my_id;
    }
    return NO_ID;
}

// O(log(n))
PersonID Datastructure::first_quartile_salary()
{
    if (map_salary_sorted.size() > 0) {
        nth_element (people_pointer_vec.begin(), people_pointer_vec.begin() + people_pointer_vec.size()/4 , people_pointer_vec.end(), nth_func);
        return people_pointer_vec.at(people_pointer_vec.size()/4)->my_id;
    }
    return NO_ID;
}

// O(log(n))
PersonID Datastructure::third_quartile_salary()
{
    if (people_pointer_vec.size() > 0) {
        nth_element (people_pointer_vec.begin(), people_pointer_vec.begin() + people_pointer_vec.size()*3/4 , people_pointer_vec.end(), nth_func);
        return people_pointer_vec.at(people_pointer_vec.size()*3/4)->my_id;
    }
    return NO_ID;
}
