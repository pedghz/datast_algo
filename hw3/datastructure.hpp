// Definition of Datastructure class, hw3 of TIE-20100/TIE-20106

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

#include <queue>
using std::priority_queue;
using std::queue;

#include <set>
using std::set;

#include <map>
using std::map;

#include <algorithm>

using namespace std;

using PersonID = string;
PersonID const NO_ID = "";

using Salary = int;
Salary const NO_SALARY = -1;

using Cost = int;
Cost const NO_COST = -1;

struct NodeRoute
{
    PersonID id_now = NO_ID;
    int distance = INFINITY;
    NodeRoute* from = nullptr;
};

struct LessDistance
{
    bool operator()(NodeRoute* lhs, NodeRoute* rhs) const
    {
        return lhs->distance > rhs->distance;
    }
};

struct Friend
{
    unordered_map<PersonID, Cost> friends_map;
};

typedef std::priority_queue<NodeRoute*, std::vector<NodeRoute*>, LessDistance> pq_type;
typedef pair<PersonID, Cost> friend_pair;
typedef unordered_map<PersonID, Friend*> unordered_map_friendship;

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

    // After this are the new operations of hw3
    void add_friendship(PersonID id, PersonID friendid, Cost cost);
    void remove_friendship(PersonID id, PersonID friendid);
    vector<pair<PersonID, Cost>> get_friends(PersonID id);
    vector<pair<PersonID, PersonID>> all_friendships();

    bool check_boss_hierarchy();
    vector<pair<PersonID, Cost>> shortest_friendpath(PersonID fromid, PersonID toid);
    vector<pair<PersonID, Cost>> cheapest_friendpath(PersonID fromid, PersonID toid);
    pair<unsigned int, Cost> leave_cheapest_friendforest();

private:
    unordered_map<PersonID, Person*> people_unordered_map;
    multimap<string, PersonID> map_name_sorted;
    multimap<Salary, PersonID> map_salary_sorted;
    vector<Person*> people_pointer_vec;
    unordered_map_friendship friendship_unordered_map;
};

#endif // DATASTRUCTURE_HPP
