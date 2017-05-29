// Definition of Datastructure class, hw3 of TIE-20100/TIE-20106

#include "datastructure.hpp"

// For debug
#include <iostream>
using std::cerr;
using std::endl;

#include <random>
using std::minstd_rand;
using std::uniform_int_distribution;

minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

void relax(NodeRoute* u,NodeRoute* v,Cost w) {
    if(v->distance > u->distance + w)
    {
        v->distance = u->distance + w;
        v->from = u;
    }
}

bool nth_func (Person* p1, Person* p2) {return p1->salary < p2->salary;}

vector<PersonID> bosses_id_list(vector<PersonID>& bosses_ids, Person* ptr)
{
    if (ptr->boss_pointer != nullptr)
    {
        bosses_ids.push_back(ptr->boss_pointer->my_id);
        bosses_id_list(bosses_ids, ptr->boss_pointer);
    }
    return bosses_ids;
}

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
    for(unordered_map_friendship::iterator fr=friendship_unordered_map.begin(); fr!=friendship_unordered_map.end(); fr++)
    {
        delete(fr->second);
    }
    friendship_unordered_map.clear();
}

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

void Datastructure::remove_person(PersonID id)
{
    unordered_map<PersonID, Person*>::iterator iter_to_removing_person_unordered_map = people_unordered_map.find(id);

    if (iter_to_removing_person_unordered_map != people_unordered_map.end())
    {
        //        ************  Friendship removal ************
        unordered_map_friendship::iterator person = friendship_unordered_map.find(id);
        if (person != friendship_unordered_map.end())
        {
            for(unordered_map<PersonID, Cost>::iterator iter=person->second->friends_map.begin(); iter!=person->second->friends_map.end(); iter++)
            {
                unordered_map_friendship::iterator tmp_person = friendship_unordered_map.find(iter->first);
                unordered_map<PersonID, Cost>::iterator removed_fr = tmp_person->second->friends_map.find(id);
                person->second->friends_map.erase(removed_fr);
            }
            Friend* p = person->second;
            friendship_unordered_map.erase(person);
            delete(p);
        }
        //                ************************************
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

        delete (iter_to_removing_person_unordered_map->second);
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

string Datastructure::get_name(PersonID id)
{
    string name = people_unordered_map.at(id)->name;
    if ( name != NO_ID ) {
        return name;
    }
    return NO_ID;
}

string Datastructure::get_title(PersonID id)
{
    string title = people_unordered_map.at(id)->title;
    if ( title != NO_ID ) {
        return title;
    }
    return NO_ID;
}

Salary Datastructure::get_salary(PersonID id)
{
    int salary = people_unordered_map.at(id)->salary;
    if ( salary != NO_SALARY ) {
        return salary;
    }
    return NO_SALARY;
}

vector<PersonID> Datastructure::find_persons(string name)
{
    vector<PersonID> employee_ids;
    pair<multimap<string, PersonID>::iterator, multimap<string, PersonID>::iterator> name_range_iter = map_name_sorted.equal_range(name);
    for (multimap<string,PersonID>::iterator it = name_range_iter.first; it != name_range_iter.second; ++it)
        employee_ids.push_back(it->second);
    sort(employee_ids.begin(), employee_ids.end());
    return employee_ids;
}

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

unsigned int Datastructure::size()
{
    return people_unordered_map.size();
}

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
    for(unordered_map_friendship::iterator fr=friendship_unordered_map.begin(); fr!=friendship_unordered_map.end(); fr++)
    {
        delete(fr->second);
    }
    friendship_unordered_map.clear();
}

vector<PersonID> Datastructure::underlings(PersonID id)
{
    Person* tmp_person = people_unordered_map.at(id);
    sort(tmp_person->underlings_vec_id.begin(), tmp_person->underlings_vec_id.end());
    return (tmp_person)->underlings_vec_id;
}

vector<PersonID> Datastructure::personnel_alphabetically()
{
    vector<PersonID> employee_ids;
    for (multimap<string,PersonID>::iterator iter = map_name_sorted.begin(); iter != map_name_sorted.end(); ++iter)
        employee_ids.push_back((*iter).second);
    return employee_ids;
}

vector<PersonID> Datastructure::personnel_salary_order()
{
    vector<PersonID> employee_ids;
    for (multimap<Salary,PersonID>::iterator iter = map_salary_sorted.begin(); iter != map_salary_sorted.end(); ++iter)
        employee_ids.push_back((*iter).second);
    return employee_ids;
}

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

PersonID Datastructure::nearest_common_boss(PersonID id1, PersonID id2)
{
    vector<PersonID> bosses_list_for_id1 = {};
    PersonID common_boss = NO_ID;
    bosses_list_for_id1 = bosses_id_list(bosses_list_for_id1, people_unordered_map.at(id1));
    common_boss = find_common_boss(bosses_list_for_id1, people_unordered_map.at(id2));
    return common_boss;
}

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

PersonID Datastructure::min_salary()
{
    if (map_salary_sorted.size() > 0) {
        multimap<Salary, PersonID>::iterator iter = map_salary_sorted.begin();
        return (*iter).second;
    }
    return NO_ID;
}

PersonID Datastructure::max_salary()
{
    if (people_pointer_vec.size() > 0) {
        multimap<Salary, PersonID>::reverse_iterator iter = map_salary_sorted.rbegin();
        return (*iter).second;
    }
    return NO_ID;
}

PersonID Datastructure::median_salary()
{
    if (people_pointer_vec.size() > 0) {
        nth_element (people_pointer_vec.begin(), people_pointer_vec.begin() + people_pointer_vec.size()/2 , people_pointer_vec.end(), nth_func);
        return people_pointer_vec.at(people_pointer_vec.size()/2)->my_id;
    }
    return NO_ID;
}

PersonID Datastructure::first_quartile_salary()
{
    if (map_salary_sorted.size() > 0) {
        nth_element (people_pointer_vec.begin(), people_pointer_vec.begin() + people_pointer_vec.size()/4 , people_pointer_vec.end(), nth_func);
        return people_pointer_vec.at(people_pointer_vec.size()/4)->my_id;
    }
    return NO_ID;
}

PersonID Datastructure::third_quartile_salary()
{
    if (people_pointer_vec.size() > 0) {
        nth_element (people_pointer_vec.begin(), people_pointer_vec.begin() + people_pointer_vec.size()*3/4 , people_pointer_vec.end(), nth_func);
        return people_pointer_vec.at(people_pointer_vec.size()*3/4)->my_id;
    }
    return NO_ID;
}

// After this are the new operations of hw3
// NOTE!!! Add all your new hw3 code here below these comments (to keep it
// separated from old hw2 code).

// O(log(V))
void Datastructure::add_friendship(PersonID id, PersonID friendid, Cost cost)
{
    unordered_map<PersonID, Person*>::iterator person_one = people_unordered_map.find(id);
    unordered_map<PersonID, Person*>::iterator person_two = people_unordered_map.find(friendid);
    if (person_one != people_unordered_map.end() && person_two != people_unordered_map.end())
    {
        Friend* fr = new Friend;
        unordered_map_friendship::iterator person_iter;
        person_iter = friendship_unordered_map.find(id);
        if (person_iter == friendship_unordered_map.end())
        {
            fr->friends_map.insert({friendid, cost});
            friendship_unordered_map.insert({id, fr});
        }
        else
        {
            (*person_iter).second->friends_map.insert({friendid, cost});
        }

        fr = new Friend;
        person_iter = friendship_unordered_map.find(friendid);
        if (person_iter == friendship_unordered_map.end())
        {
            fr->friends_map.insert({id, cost});
            friendship_unordered_map.insert({friendid, fr});
        }
        else
        {
            (*person_iter).second->friends_map.insert({id, cost});
        }
    }
}

// O(log(V))
void Datastructure::remove_friendship(PersonID id, PersonID friendid)
{
    unordered_map<PersonID, Person*>::iterator person_one = people_unordered_map.find(id);
    unordered_map<PersonID, Person*>::iterator person_two = people_unordered_map.find(friendid);
    unordered_map_friendship::iterator person_one_friends = friendship_unordered_map.find(id);
    unordered_map_friendship::iterator person_two_friends = friendship_unordered_map.find(friendid);
    if (person_one != people_unordered_map.end() && person_two != people_unordered_map.end()
            && person_one_friends != friendship_unordered_map.end() && person_two_friends != friendship_unordered_map.end())
    {
        unordered_map<PersonID, Cost>::iterator remove_it = person_one_friends->second->friends_map.find(friendid);
        if (remove_it != person_one_friends->second->friends_map.end())
            person_one_friends->second->friends_map.erase(remove_it);

        remove_it = person_two_friends->second->friends_map.find(id);
        if (remove_it != person_two_friends->second->friends_map.end())
            person_two_friends->second->friends_map.erase(remove_it);
    }
}

// O(V)
vector<pair<PersonID, Cost>> Datastructure::get_friends(PersonID id)
{
    unordered_map_friendship::iterator person_iter = friendship_unordered_map.find(id);
    vector<pair<PersonID, Cost>> friends_vector = {};
    if (person_iter != friendship_unordered_map.end())
    {
        for(unordered_map<PersonID, Cost>::iterator iter=person_iter->second->friends_map.begin(); iter!=person_iter->second->friends_map.end(); iter++)
        {
            friends_vector.push_back({iter->first, iter->second});
        }
    }
    sort(friends_vector.begin(), friends_vector.end());
    return friends_vector; // Replace this with the actual implementation
}

// O(E)
vector<pair<PersonID, PersonID>> Datastructure::all_friendships()
{
    vector<pair<PersonID, PersonID>> all_friendships = {};
    for (unordered_map_friendship::iterator iter=friendship_unordered_map.begin(); iter!=friendship_unordered_map.end(); iter++)
    {
        for(unordered_map<PersonID, Cost>::iterator iter2=iter->second->friends_map.begin(); iter2!=iter->second->friends_map.end(); iter2++)
        {
            if (iter->first <= iter2->first)
                all_friendships.push_back({iter->first, iter2->first});
        }
    }
    sort(all_friendships.begin(), all_friendships.end());
    return all_friendships;
}

void empty_q(queue<NodeRoute*>& que)
{
    NodeRoute* tmp;
    while (!que.empty())
    {
        tmp = que.front();
        que.pop();
        delete tmp;
    }
}

// O(V + E*log(V))
vector<pair<PersonID, Cost> > Datastructure::shortest_friendpath(PersonID fromid, PersonID toid)
{    
    vector<pair<PersonID, Cost>> vec_path = {};
    queue<NodeRoute*> que;
    set<PersonID> visited_people;
    NodeRoute* u= new NodeRoute;
    u->id_now = fromid;
    u->distance = 0;
    que.push(u);
    visited_people.insert(fromid);

    while (!que.empty())
    {
        u = que.front();
        que.pop();
        if (u->id_now == toid)
        {
            NodeRoute *tmp;
            while (u->id_now != fromid)
            {
                vec_path.push_back({u->id_now, u->distance - u->from->distance});
                tmp = u;
                u = u->from;
                delete tmp;
            }
            empty_q(que);
            reverse(vec_path.begin(),vec_path.end());
            return vec_path;
        }
        unordered_map_friendship::iterator person_iter = friendship_unordered_map.find(u->id_now);
        if (person_iter != friendship_unordered_map.end())
            for (unordered_map<PersonID, Cost>::iterator iter=person_iter->second->friends_map.begin(); iter!=person_iter->second->friends_map.end(); ++iter)
            {
                NodeRoute* v= new NodeRoute;
                v->id_now = iter->first;
                if (visited_people.find(iter->first) == visited_people.end())
                {
                    visited_people.insert(v->id_now);
                    v->from = u;
                    v->distance = u->distance + iter->second;
                    que.push(v);
                }
            }
    }
    empty_q(que);
    return {};
}

pair<bool, int> DFS_underlings(unordered_map<PersonID, Person*>& people_unordered_map, set<PersonID>& visited_people,Person* person, int counter)
{
    bool result = true;
    pair<bool, int> res;
    for (vector<PersonID>::iterator iter=person->underlings_vec_id.begin(); iter!=person->underlings_vec_id.end(); iter++)
    {
        Person* tmp_person = people_unordered_map.find(*iter)->second;
        if (visited_people.find(*iter) != visited_people.end())
            return {false,counter};
        visited_people.insert(*iter);
        counter++;
        res = DFS_underlings(people_unordered_map, visited_people, tmp_person, counter);
        counter = res.second;
        result = result & res.first;
        if (result == false)
            return {false,counter};
    }
    return {result, counter};
}

pair<bool, int> DFS_boss(unordered_map<PersonID, Person*>& people_unordered_map, set<PersonID>& visited_people,Person* person, int counter)
{
    bool result = true;
    pair<bool, int> res;
    if (person->boss_pointer != nullptr)
    {
        Person* boss = person->boss_pointer;
        for (vector<PersonID>::iterator iter=boss->underlings_vec_id.begin(); iter!=boss->underlings_vec_id.end(); iter++)
        {
            if(*iter != person->my_id)
            {
                Person* tmp_person = people_unordered_map.find(*iter)->second;
                if (visited_people.find(*iter) != visited_people.end())
                    return {false,counter};
                visited_people.insert(*iter);
                counter++;
                res = DFS_underlings(people_unordered_map, visited_people, tmp_person, counter);
                counter = res.second;
                result = result & res.first ;
                if (result == false)
                    return {false,counter};
            }
        }
        if (visited_people.find(boss->my_id) != visited_people.end())
            return {false,counter};
        counter++;
        visited_people.insert(boss->my_id);
        res = DFS_boss(people_unordered_map, visited_people, boss, counter);
        counter = res.second;
        result = result & res.first;
        if (result == false)
            return {false,counter};
    }
    return {result, counter};
}

// O(E)
bool Datastructure::check_boss_hierarchy()
{
    if (people_unordered_map.empty())
        return true;
    set<PersonID> visited_people = {};
    Person* person = people_unordered_map.begin()->second;
    visited_people.insert(person->my_id);
    pair<bool,int> result1 = {false,1};
    pair<bool,int> result2 = {false,1};
    int counter = 1;
    result1 = DFS_underlings(people_unordered_map, visited_people, person, counter);
    result2 = DFS_boss(people_unordered_map, visited_people, person, result1.second);
    int size = people_pointer_vec.size();
    if (result2.second != size)
        return false;
    return result1.first & result2.first;
}

void empty_pq(pq_type& priority_Q)
{
    NodeRoute* tmp;
    while (!priority_Q.empty())
    {
        tmp = priority_Q.top();
        priority_Q.pop();
        delete tmp;
    }
}

// O(V + E*log(V))
vector<pair<PersonID, Cost>> Datastructure::cheapest_friendpath(PersonID fromid, PersonID toid)
{
    vector<pair<PersonID, Cost>> vec_path = {};
    pq_type priority_Q;
    NodeRoute *u= new NodeRoute;
    u->id_now = fromid;
    u->distance = 0;
    priority_Q.push(u);

    set<PersonID> visited_people;
    visited_people.insert(fromid);

    while (!priority_Q.empty())
    {
        u = priority_Q.top();
        priority_Q.pop();
        visited_people.insert(u->id_now);
        if (u->id_now == toid)
        {
            NodeRoute* tmp;
            while (u->id_now != fromid)
            {
                vec_path.push_back({u->id_now, u->distance - u->from->distance});
                tmp = u;
                u = u->from;
                delete tmp;
            }
            empty_pq(priority_Q);
            reverse(vec_path.begin(),vec_path.end());
            return vec_path;
        }
        unordered_map_friendship::iterator person_iter = friendship_unordered_map.find(u->id_now);
        if (person_iter != friendship_unordered_map.end())
            for (unordered_map<PersonID, Cost>::iterator iter=person_iter->second->friends_map.begin(); iter!=person_iter->second->friends_map.end(); ++iter)
            {
                NodeRoute *v= new NodeRoute;
                v->id_now = iter->first;
                if (visited_people.find(iter->first) == visited_people.end())
                {
                    relax(u, v, iter->second);
                    priority_Q.push(v);
                }
            }
    }
    empty_pq(priority_Q);
    return {};
}

PersonID find_set(PersonID& person, vector<pair<PersonID, PersonID>>& disjoint_vec)
{
    for(vector<pair<PersonID, PersonID>>::iterator iter=disjoint_vec.begin(); iter!=disjoint_vec.end(); iter++)
    {
        if(iter->first == person)
            return iter->second;
    }
    return "";
}

Cost merge(PersonID& one, PersonID& two, vector<pair<PersonID, PersonID>>& disjoint_vec,
           unordered_map<PersonID, Friend*>& friendship_unordered_map,
           unordered_map<PersonID, Friend*>& friendship_unordered_map_copy, Cost cost)
{
    // Put one and two into the same component.
    // (Put same label for both of them).
    PersonID p1label = find_set(one, disjoint_vec);
    PersonID p2label = find_set(two, disjoint_vec);

    // Nothing to do if one and two are already
    //in the same component(have the same label).
    if(p1label == p2label)
        return cost;

    // Rename two's label to one’s label.
    for (vector<pair<PersonID, PersonID>>::iterator iter=disjoint_vec.begin(); iter!=disjoint_vec.end(); iter++)
    {
        if (iter->second == p1label)
            iter->second = p2label;
    }

    // Adding the added Friendship to friendship_unordered_map_copy and returning the cost till now.
    Friend* id_iter;
    Cost weight;
    id_iter = friendship_unordered_map.at(one);
    unordered_map<PersonID, Cost>::iterator friend_iter = id_iter->friends_map.find(two);
    if (friend_iter != id_iter->friends_map.end())
        weight = friend_iter->second;
    cost = cost + weight;

    Friend* fr = new Friend;
    unordered_map_friendship::iterator person_iter = friendship_unordered_map_copy.find(one);

    if (person_iter == friendship_unordered_map_copy.end())
    {
        fr->friends_map.insert({two, weight});
        friendship_unordered_map_copy.insert({one, fr});
    }
    else
    {
        (*person_iter).second->friends_map.insert({two, weight});
    }

    fr = new Friend;
    person_iter = friendship_unordered_map_copy.find(two);
    if (person_iter == friendship_unordered_map_copy.end())
    {
        fr->friends_map.insert({one, weight});
        friendship_unordered_map_copy.insert({two, fr});
    }
    else
    {
        (*person_iter).second->friends_map.insert({one, weight});
    }
    return cost;
}

// O(V*E)
pair<unsigned int, Cost> Datastructure::leave_cheapest_friendforest()
{
    vector<pair<PersonID, PersonID>> disjoint_vec;
    multimap<Cost, pair<PersonID, PersonID>> multi_orderd_friendship;
    Cost cost = 0;

    // Ordering Friendships by their cost + Making the disjoint set vector with default label of its own id for wahc node.
    for(unordered_map<PersonID, Friend*>::iterator person=friendship_unordered_map.begin(); person!=friendship_unordered_map.end(); person++)
    {
        disjoint_vec.push_back({person->first, person->first});
        for(unordered_map<PersonID, Cost>::iterator iter=person->second->friends_map.begin(); iter!=person->second->friends_map.end(); iter++)
            multi_orderd_friendship.insert({iter->second, {person->first, iter->first} });
    }

    // Making a new container for storing the cheapest friendship tree
    unordered_map<PersonID, Friend*> friendship_unordered_map_copy = {};

    // Implementig Kruskal algorithm with disjoint sets - this parts complexity is really bad!
    for(multimap<Cost, pair<PersonID, PersonID>>::iterator iter=multi_orderd_friendship.begin(); iter!=multi_orderd_friendship.end(); iter++)
    {
        cost = merge(iter->second.first, iter->second.second, disjoint_vec, friendship_unordered_map, friendship_unordered_map_copy, cost);
    }

    // Finding the number of separated trees by counting the number of different sets;
    set<PersonID> sets = {};
    for(pair<PersonID, PersonID> set : disjoint_vec)
    {
        if (sets.find(set.second) == sets.end())
            sets.insert(set.second);
    }
    unsigned int count = sets.size();

    // Clearing the original friendship container
    for(unordered_map_friendship::iterator fr=friendship_unordered_map.begin(); fr!=friendship_unordered_map.end(); fr++)
    {
        delete(fr->second);
    }
    friendship_unordered_map.clear();

    // Moving remained friendships to its original container
    for(unordered_map<PersonID, Friend*>::iterator iter=friendship_unordered_map_copy.begin(); iter!=friendship_unordered_map_copy.end(); iter++)
    {
        friendship_unordered_map.insert(*iter);
    }

    return {count, cost};
}
