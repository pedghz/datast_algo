// The main program (provided by the course), hw1 of TIE-20100/TIE-20106
//
// DO ****NOT**** EDIT THIS FILE!
// (Preferably do not edit this even temporarily. And if you still decide to do so
//  (for debugging, for example), DO NOT commit any changes to git, but revert all
//  changes later. Otherwise you won't be able to get any updates/fixes to this
//  file from git!)

#include "datastructure.hpp"

#include <string>
using std::string;
using std::getline;

#include <iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::flush;
using std::noskipws;

#include <istream>
using std::istream;

#include <ostream>
using std::ostream;

#include <fstream>
using std::ifstream;

#include <sstream>
using std::istringstream;
using std::stringstream;

#include <iomanip>
using std::setw;

#include <stdexcept>
using std::invalid_argument;

#include <tuple>
using std::tuple;
using std::make_tuple;
using std::get;
using std::tie;

#include <regex>
using std::regex_match;
using std::regex_search;
using std::smatch;
using std::regex;
using std::sregex_token_iterator;

#include <algorithm>
using std::find_if;
using std::find;
using std::max_element;

#include <random>
using std::minstd_rand;
using std::uniform_int_distribution;

#include <chrono>

#include <functional>
using std::function;
using std::equal_to;

#include <vector>
using std::vector;

#include <array>
using std::array;

#include <ctime>
using std::time;

#include <cstdlib>
#include <cassert>


namespace
{
minstd_rand rand_engine;

template <typename Type>
Type random(Type start, Type end)
{
    auto range = end-start;
    assert(range != 0 && "random() with zero range!");

    auto num = uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

const array<unsigned long int, 20> primes1{4943,   4951,   4957,   4967,   4969,   4973,   4987,   4993,   4999,   5003,
                                          5009,   5011,   5021,   5023,   5039,   5051,   5059,   5077,   5081,   5087};
const array<unsigned long int, 20> primes2{81031,  81041,  81043,  81047,  81049,  81071,  81077,  81083,  81097,  81101,
                                           81119,  81131,  81157,  81163,  81173,  81181,  81197,  81199,  81203,  81223};
unsigned long int prime1 = 0; // Will be initialized to random value from above in main
unsigned long int prime2 = 0; // Will be initialized to random value from above in main
unsigned long int people_added = 0; // Counter for random people added (to keep ids unique)

enum class TestStatus { NOT_RUN, NO_DIFFS, DIFFS_FOUND };
TestStatus test_status = TestStatus::NOT_RUN;

void init_primes()
{
    // Initialize id generator
    prime1 = primes1[random<int>(0, primes1.size())];
    prime2 = primes2[random<int>(0, primes2.size())];
    people_added = 0;
}

PersonID n_to_id(unsigned long int n)
{
    unsigned long int hash = prime1*n + prime2;
    PersonID id;

    while (hash > 0)
    {
        auto hexnum = hash % 16;
        hash /= 16;
        id.push_back((hexnum < 10) ? ('0'+hexnum) : ('a'+hexnum-10));
    }

    return id;
}

//unsigned int startmem = 0;
using PersonList = vector<PersonID>;

using MatchIter = smatch::const_iterator;

enum class StopwatchMode { OFF, ON, NEXT };
StopwatchMode stopwatch_mode = StopwatchMode::OFF;

class Stopwatch
{
public:
    using Clock = std::chrono::high_resolution_clock;

    Stopwatch() : running(false) { }
    void start() { running = true; starttime = Clock::now(); }
    void stop() { endtime = Clock::now(); running = false; }
    double elapsed()
    {
        if (!running)
        {
            std::chrono::duration<double> elapsed = endtime - starttime;
            return elapsed.count();
        }
        else
        {
            std::chrono::duration<double> elapsed = Clock::now() - starttime;
            return elapsed.count();
        }
    }
private:
    std::chrono::time_point<Clock> starttime;
    std::chrono::time_point<Clock> endtime;
    bool running;
};

template <typename To>
To convert_string_to(string from)
{
    istringstream istr(from);
    To result;
    istr >> noskipws >> result;
    if (istr.fail() || !istr.eof())
    {
        throw invalid_argument("Cannot convert string to required type");
    }
    return result;
}

//std::tuple<unsigned long int,string> mempeak(){
//    ifstream file_stream("/proc/self/status");
//    if( file_stream ){
//        string line;
//        string ID;
//        unsigned long int amnt;
//        string unit;
//        while( getline( file_stream, line ))
//        {
//            istringstream line_stream( line );
//            getline(line_stream, ID, ':');
//            if (ID == "VmPeak"){
//                line_stream >> amnt;
//                line_stream >> unit;
//                return make_tuple(amnt, unit);
//            }
//        }
//    }
//    file_stream.close();
//    return make_tuple(0, "?");
//}

void add_random_persons(Datastructure& ds, unsigned int size, bool even_boss = false)
{
    for (unsigned int i = 0; i < size; ++i)
    {
        string id = n_to_id(people_added);

        string name;
        for (unsigned int j = 0; j < 4; ++j)
        {
            name += random('a', 'k'); // 10 letters
        }

        Salary salary = random<Salary>(1, 10000);

        string title;
        switch (random<int>(0,5))
        {
        case 0:
            title = "Software developer";
            break;
        case 1:
            title = "Web designer";
            break;
        case 2:
            title = "Experience evangelist";
            break;
        case 3:
            title = "General boss";
            break;
        case 4:
            title = "Useless idiot";
            break;
        }

        ds.add_person(name, id, title, salary);

        // Add random boss whose number is smaller (0 = ceo, don't add). And even, if so requested (in perftest, odd people may have been removed)
        if (people_added > 0)
        {
            PersonID bossid;
            if (even_boss) { bossid = n_to_id(random<decltype(people_added)>(0,(people_added+1)/2)*2); }
            else { bossid = n_to_id(random<decltype(people_added)>(0,people_added)); }
            ds.add_boss(id, bossid);
        }

        ++people_added;
    }
}

enum class PromptStyle { NORMAL, NO_ECHO, NO_NESTING };
void command_parser(Datastructure& ds, istream& input, ostream& output, PromptStyle promptstyle);

void help_command(Datastructure& ds, ostream& /*output*/, MatchIter /*begin*/, MatchIter /*end*/);

void test_get_functions(Datastructure& ds, PersonID id)
{
    ds.get_name(id);
    ds.get_salary(id);
    ds.get_title(id);
}

void cmd_add(Datastructure& ds, ostream& /*output*/, MatchIter begin, MatchIter end)
{
    string name = *begin++;
    string id = *begin++;
    string title = *begin++;
    string salarystr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    Salary salary = convert_string_to<Salary>(salarystr);

    ds.add_person(name, id, title, salary);
}

void cmd_remove(Datastructure& ds, ostream& /*output*/, MatchIter begin, MatchIter end)
{
    string id = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    ds.remove_person(id);
}

void test_remove(Datastructure& ds)
{
    // Choose random *odd* number to remove
    if (people_added > 1) // Don't remove ceo if the only person
    {
        auto id = n_to_id(random<decltype(people_added)>(0, people_added/2)*2 +1);
        ds.remove_person(id);
    }
}

void cmd_add_boss(Datastructure& ds, ostream& /*output*/, MatchIter begin, MatchIter end)
{
    string id = *begin++;
    string bossid = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    ds.add_boss(id, bossid);
}

void cmd_higher_lower_ranks(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string id = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    auto result = ds.higher_lower_ranks(id);
    output << "Persons with higher rank (closer to ceo): " << result.first << endl;
    output << "Persons with lower rank (further away from ceo): " << result.second << endl;
}

void test_higher_lower_ranks(Datastructure& ds)
{
    if (people_added > 0)
    {
        // Choose random *even* number to operate
        auto id = n_to_id(random<decltype(people_added)>(0, (people_added+1)/2)*2);

        test_get_functions(ds, id);

        ds.higher_lower_ranks(id);
    }
}

void cmd_random_add(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string sizestr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    unsigned int size = convert_string_to<unsigned int>(sizestr);

    add_random_persons(ds, size);

    output << "Added: " << size << " persons" << endl;
}

void cmd_randseed(Datastructure& /*ds*/, ostream& output, MatchIter begin, MatchIter end)
{
    string seedstr = *begin++;
    assert(begin == end && "Invalid number of parameters");

    unsigned long int seed = convert_string_to<unsigned long int>(seedstr);

    rand_engine.seed(seed);
    init_primes();

    output << "Random seed set to " << seed << endl;
}

void cmd_read(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string filename = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    ifstream input(filename);
    if (input)
    {
        output << "** Commands from '" << filename << "'" << endl;
        command_parser(ds, input, output, PromptStyle::NORMAL);
        output << "** End of commands from '" << filename << "'" << endl;
    }
    else
    {
        output << "Cannot open file '" << filename << "'!" << endl;
    }
}

void cmd_testread(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string infilename = *begin++;
    string outfilename = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    ifstream input(infilename);
    if (input)
    {
        ifstream expected_output(outfilename);
        if (output)
        {
            stringstream actual_output;
            command_parser(ds, input, actual_output, PromptStyle::NO_NESTING);

            vector<string> actual_lines;
            while (actual_output)
            {
                string line;
                getline(actual_output, line);
                if (!actual_output) { break; }
                actual_lines.push_back(line);
            }

            vector<string> expected_lines;
            while (expected_output)
            {
                string line;
                getline(expected_output, line);
                if (!expected_output) { break; }
                expected_lines.push_back(line);
            }

            string heading_actual = "Actual output";
            unsigned int actual_max_length = heading_actual.length();
            auto actual_max_iter = std::max_element(actual_lines.cbegin(), actual_lines.cend(),
                                                    [](string s1, string s2){ return s1.length() < s2.length(); });
            if (actual_max_iter != actual_lines.cend())
            {
                actual_max_length = actual_max_iter->length();
            }

            string heading_expected = "Expected output";
            unsigned int expected_max_length = heading_expected.length();
            auto expected_max_iter = std::max_element(expected_lines.cbegin(), expected_lines.cend(),
                                                    [](string s1, string s2){ return s1.length() < s2.length(); });
            if (expected_max_iter != expected_lines.cend())
            {
                expected_max_length = expected_max_iter->length();
            }

            auto pos_actual = actual_lines.cbegin();
            auto pos_expected = expected_lines.cbegin();
            output << "  " << heading_actual << string(actual_max_length - heading_actual.length(), ' ') << " | " << heading_expected << endl;
            output << "--" << string(actual_max_length, '-') << "-|-" << string(expected_max_length, '-') << endl;

            bool lines_ok = true;
            while (pos_expected != expected_lines.cend() || pos_actual != actual_lines.cend())
            {
                if (pos_expected != expected_lines.cend())
                {
                    if (pos_actual != actual_lines.cend())
                    {
                        bool ok = (*pos_expected == *pos_actual);
                        output << (ok ? ' ' : '?') << ' ' << *pos_actual << string(actual_max_length - pos_actual->length(), ' ')
                               << " | " << *pos_expected << endl;
                        lines_ok = lines_ok && ok;
                        ++pos_actual;
                    }
                    else
                    { // Actual output was too short
                        output << "? " << string(actual_max_length, ' ')
                               << " | " << *pos_expected << endl;
                        lines_ok = false;
                    }
                    ++pos_expected;
                }
                else
                { // Actual output was too long
                    output << "? " << *pos_actual << string(actual_max_length - pos_actual->length(), ' ')
                           << " | " << endl;
                    lines_ok = false;
                    ++pos_actual;
                }
            }
            if (lines_ok)
            {
                output << "**No differences in output.**" << endl;
                if (test_status == TestStatus::NOT_RUN)
                {
                    test_status = TestStatus::NO_DIFFS;
                }
            }
            else
            {
                output << "**Differences found! (Lines beginning with '?')**" << endl;
                test_status = TestStatus::DIFFS_FOUND;
            }

        }
        else
        {
            output << "Cannot open file '" << outfilename << "'!" << endl;
        }
    }
    else
    {
        output << "Cannot open file '" << infilename << "'!" << endl;
    }
}

void cmd_size(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    assert( begin == end && "Impossible number of parameters!");

    output << "Number of employees: " << ds.size() << endl;
}

void cmd_stopwatch(Datastructure& /*ds*/, ostream& output, MatchIter begin, MatchIter end)
{
    string on = *begin++;
    string off = *begin++;
    string next = *begin++;
    assert(begin == end && "Invalid number of parameters");

    if (!on.empty())
    {
        stopwatch_mode = StopwatchMode::ON;
        output << "Stopwatch: on" << endl;
    }
    else if (!off.empty())
    {
        stopwatch_mode = StopwatchMode::OFF;
        output << "Stopwatch: off" << endl;
    }
    else if (!next.empty())
    {
        stopwatch_mode = StopwatchMode::NEXT;
        output << "Stopwatch: on for the next command" << endl;
    }
    else
    {
        assert(!"Impossible stopwatch mode!");
    }
}

void cmd_clear(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    assert(begin == end && "Invalid number of parameters");

    ds.clear();
    init_primes();

    output << "Cleared all persons" << endl;
}

void print_person(Datastructure& ds, PersonID id, ostream& output)
{
    if (id != NO_ID)
    {
        output << "id " << id << " : " << ds.get_title(id) << " " << ds.get_name(id) << ", salary " << ds.get_salary(id);
    }
    else
    {
        output << "-- unknown --";
    }
}

void print_person_list(Datastructure& ds, PersonList const& person_list, ostream& output)
{
    for (auto person : person_list)
    {
        print_person(ds, person, output);
        output << endl;
    }
}

void cmd_change_name(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string id = *begin++;
    string name = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    ds.change_name(id, name);
    print_person(ds, id, output);
    output << endl;
}

void test_change_name(Datastructure& ds)
{
    if (people_added > 0)
    {
        // Choose random *even* number to operate
        auto id = n_to_id(random<decltype(people_added)>(0, (people_added+1)/2)*2);

        string name;
        for (unsigned int j = 0; j < 4; ++j)
        {
            name += random('a', 'k'); // 10 letters
        }

        ds.change_name(id, name);

        test_get_functions(ds, id);
    }
}

void cmd_change_salary(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string id = *begin++;
    string salarystr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    Salary salary = convert_string_to<Salary>(salarystr);

    ds.change_salary(id, salary);
    print_person(ds, id, output);
    output << endl;
}

void test_change_salary(Datastructure& ds)
{
    if (people_added > 0)
    {
        // Choose random *even* number to operate
        auto id = n_to_id(random<decltype(people_added)>(0, (people_added+1)/2)*2);

        Salary salary = random<Salary>(1, 10000);

        ds.change_salary(id, salary);

        test_get_functions(ds, id);
    }
}

void cmd_titlelist(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string title = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    print_person_list(ds, ds.personnel_with_title(title), output);
}

//void test_titlelist(Datastructure& ds)
//{
//    string title;
//    switch (random<int>(0,5))
//    {
//    case 0:
//        title = "Software developer";
//        break;
//    case 1:
//        title = "Web designer";
//        break;
//    case 2:
//        title = "Experience evangelist";
//        break;
//    case 3:
//        title = "General boss";
//        break;
//    case 4:
//        title = "Useless idiot";
//        break;
//    }

//    ds.personnel_with_title(title);
//}

void cmd_find(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string name = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    print_person_list(ds, ds.find_persons(name), output);
}

void test_find(Datastructure& ds)
{
    string name;
    for (unsigned int j = 0; j < 4; ++j)
    {
        name += random('a', 'k'); // 10 letters
    }

    auto result = ds.find_persons(name);

    for (auto& i : result)
    {
        test_get_functions(ds, i);
    }
}

void cmd_nearest_common_boss(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string id1 = *begin++;
    string id2 = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    print_person(ds, ds.nearest_common_boss(id1, id2), output);
    output << endl;
}

void test_nearest_common_boss(Datastructure& ds)
{
    if (people_added > 0)
    {
        // Choose two random *even* numbers to operate
        auto id1 = n_to_id(random<decltype(people_added)>(0, (people_added+1)/2)*2);
        auto id2 = n_to_id(random<decltype(people_added)>(0, (people_added+1)/2)*2);

        ds.nearest_common_boss(id1, id2);

        test_get_functions(ds, id1);
    }
}

void print_underlings_hierarchy(Datastructure& ds, PersonID id, unsigned int level, ostream& output)
{
    output << string(level, ' ');
    print_person(ds, id, output);
    output << endl;

    for (auto i : ds.underlings(id))
    {
        print_underlings_hierarchy(ds, i, level+1, output);
    }
}

void cmd_underlings(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string id = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    print_underlings_hierarchy(ds, id, 0, output);
}

void cmd_add_friendship(Datastructure& ds, ostream& /*output*/, MatchIter begin, MatchIter end)
{
    string id = *begin++;
    string friendid = *begin++;
    string coststr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    Cost cost = convert_string_to<Cost>(coststr);

    ds.add_friendship(id, friendid, cost);
}

void add_random_friendships(Datastructure& ds, unsigned int friend_count)
{
    if (people_added > 1)
    {
        while (friend_count != 0)
        {
            // Choose two random numbers to operate
            auto id1 = n_to_id(random<decltype(people_added)>(0, people_added));
            auto id2 = n_to_id(random<decltype(people_added)>(0, people_added));
            if (id1 != id2)
            {
                auto cost = random<Cost>(0, 10);
                ds.add_friendship(id1, id2, cost);
            }
            --friend_count;
        }
    }
}

void cmd_remove_friendship(Datastructure& ds, ostream& /*output*/, MatchIter begin, MatchIter end)
{
    string id = *begin++;
    string friendid = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    ds.remove_friendship(id, friendid);
}

void test_remove_friendship(Datastructure& ds)
{
    if (people_added > 1)
    {
        // Choose two random numbers to operate
        auto id1 = n_to_id(random<decltype(people_added)>(0, people_added));
        auto id2 = n_to_id(random<decltype(people_added)>(0, people_added));
        if (id1 != id2)
        {
            ds.remove_friendship(id1, id2);
        }
    }
}

void cmd_get_friends(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string id = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    auto result = ds.get_friends(id);

    for (auto const& i : result)
    {
        print_person(ds, i.first, output);
        output << " (cost " << i.second << ")" << endl;
    }
}

void test_get_friends(Datastructure& ds)
{
    if (people_added > 0)
    {
        // Choose random person
        auto id = n_to_id(random<decltype(people_added)>(0, people_added));
        ds.get_friends(id);
    }
}

void cmd_all_friends(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    assert( begin == end && "Impossible number of parameters!");

    auto result = ds.all_friendships();

    for (auto const& p : result)
    {
        output << p.first << " - " << p.second << endl;
    }
}

void cmd_add_random_friends(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string numberstr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    unsigned int number = convert_string_to<unsigned int>(numberstr);

    add_random_friendships(ds, number);

    output << "Added: " << number << " friendships" << endl;
}

void cmd_shortest_friendpath(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string fromid = *begin++;
    string toid = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    auto result = ds.shortest_friendpath(fromid, toid);

    if (!result.empty())
    {
        output << "Shortest path of friends is:" << endl;
        Cost total_cost = 0;
        for (auto const& i : result)
        {
            print_person(ds, i.first, output);
            output << " (cost " << i.second << ")" << endl;
            total_cost += i.second;
        }
        output << "Total cost is " << total_cost << endl;
    }
    else
    {
        output << "No path found." << endl;
    }
}

void test_shortest_friendpath(Datastructure& ds)
{
    if (people_added > 0)
    {
        // Choose two random persons
        auto id1 = n_to_id(random<decltype(people_added)>(0, people_added));
        auto id2 = n_to_id(random<decltype(people_added)>(0, people_added));
        ds.shortest_friendpath(id1, id2);
    }
}

void cmd_check_boss_hierarchy(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    assert( begin == end && "Impossible number of parameters!");

    bool ok = ds.check_boss_hierarchy();

    output << "Boss hierarchy is " << (ok ? "ok." : "broken.") << endl;
}

void test_check_boss_hierarchy(Datastructure& ds)
{
    ds.check_boss_hierarchy();
}

void cmd_cheapest_friendpath(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string fromid = *begin++;
    string toid = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    auto result = ds.cheapest_friendpath(fromid, toid);

    if (!result.empty())
    {
        output << "Cheapest path of friends is:" << endl;
        Cost total_cost = 0;
        for (auto const& i : result)
        {
            print_person(ds, i.first, output);
            output << " (cost " << i.second << ")" << endl;
            total_cost += i.second;
        }
        output << "Total cost is " << total_cost << endl;
    }
    else
    {
        output << "No path found." << endl;
    }
}

void test_cheapest_friendpath(Datastructure& ds)
{
    if (people_added > 0)
    {
        // Choose two random persons
        auto id1 = n_to_id(random<decltype(people_added)>(0, people_added));
        auto id2 = n_to_id(random<decltype(people_added)>(0, people_added));
        ds.cheapest_friendpath(id1, id2);
    }
}

void cmd_leave_cheapest_friendforest(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    assert( begin == end && "Impossible number of parameters!");

    auto result = ds.leave_cheapest_friendforest();

    output << "Remaining friend forest has " << result.first << " trees with the total cost of " << result.second << endl;
}

void test_leave_cheapest_friendforest(Datastructure& ds)
{
    ds.leave_cheapest_friendforest();
}

template<PersonID(Datastructure::*MFUNC)()>
void NoParPersonCmd(Datastructure& ds, ostream& output, MatchIter /*begin*/, MatchIter /*end*/)
{
    auto result = (ds.*MFUNC)();
    print_person(ds, result, output);
    output << endl;
}

template<PersonList(Datastructure::*MFUNC)()>
void NoParListCmd(Datastructure& ds, ostream& output, MatchIter /*begin*/, MatchIter /*end*/)
{
    auto result = (ds.*MFUNC)();
    print_person_list(ds, result, output);
}

void cmd_perftest(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end);

struct CmdInfo
{
    string cmd;
    string param_regex_str;
    function<void(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)> func;
    function<void(Datastructure& ds)> testfunc;
};

vector<CmdInfo> cmds =
{
    {"add", "'([a-zA-Z ]+)'[[:space:]]+([0-9A-Za-z]+)[[:space:]]+'([a-zA-Z ]+)'[[:space:]]+([0-9]+)", &cmd_add, nullptr },
    {"remove", "([0-9A-Za-z]+)", &cmd_remove, &test_remove },
    {"random_add", "([0-9]+)", &cmd_random_add, [](auto& ds){add_random_persons(ds, 1, true);} },
    {"find", "'([a-zA-Z ]+)'", &cmd_find, &test_find },
    {"change_name", "([0-9A-Za-z]+)[[:space:]]+'([a-zA-Z ]+)'", &cmd_change_name, &test_change_name },
    {"change_salary", "([0-9A-Za-z]+)[[:space:]]+([0-9]+)", &cmd_change_salary, &test_change_salary },
    {"add_boss", "([0-9A-Za-z]+)[[:space:]]+([0-9A-Za-z]+)", &cmd_add_boss, nullptr },
    {"size", "", &cmd_size, nullptr },
    {"clear", "", &cmd_clear, nullptr },
    {"underlings", "([0-9A-Za-z]+)", &cmd_underlings, nullptr },
    {"titlelist", "'([a-zA-Z ]+)'", &cmd_titlelist, nullptr },
    {"alphalist", "", &NoParListCmd<&Datastructure::personnel_alphabetically>, [](auto& ds){ds.personnel_alphabetically();} },
    {"salarylist", "", &NoParListCmd<&Datastructure::personnel_salary_order>, [](auto& ds){ds.personnel_salary_order();} },
    {"ceo", "", &NoParPersonCmd<&Datastructure::find_ceo>, [](auto& ds){ds.find_ceo();} },
    {"nearest_common_boss", "([0-9A-Za-z]+)[[:space:]]+([0-9A-Za-z]+)", &cmd_nearest_common_boss, &test_nearest_common_boss },
    {"higher_lower_ranks", "([0-9A-Za-z]+)", &cmd_higher_lower_ranks, &test_higher_lower_ranks },
    {"min", "", &NoParPersonCmd<&Datastructure::min_salary>, [](auto& ds){ds.min_salary();} },
    {"max", "", &NoParPersonCmd<&Datastructure::max_salary>, [](auto& ds){ds.max_salary();} },
    {"median", "", &NoParPersonCmd<&Datastructure::median_salary>, [](auto& ds){ds.median_salary();} },
    {"1stquartile", "", &NoParPersonCmd<&Datastructure::first_quartile_salary>, [](auto& ds){ds.first_quartile_salary();} },
    {"3rdquartile", "", &NoParPersonCmd<&Datastructure::third_quartile_salary>, [](auto& ds){ds.third_quartile_salary();} },
    {"add_friend", "([0-9A-Za-z]+)[[:space:]]+([0-9A-Za-z]+)[[:space:]]+([0-9]+)", &cmd_add_friendship, nullptr},
    {"remove_friend", "([0-9A-Za-z]+)[[:space:]]+([0-9A-Za-z]+)", &cmd_remove_friendship, &test_remove_friendship },
    {"friends_of", "([0-9A-Za-z]+)", &cmd_get_friends, &test_get_friends },
    {"all_friendships", "", &cmd_all_friends, nullptr },
    {"random_friends", "([0-9]+)", &cmd_add_random_friends, [](auto& ds){ add_random_friendships(ds, 5); } },
    {"shortest_friendpath", "([0-9A-Za-z]+)[[:space:]]+([0-9A-Za-z]+)", &cmd_shortest_friendpath, &test_shortest_friendpath },
    {"check_boss_hierarchy", "", &cmd_check_boss_hierarchy, &test_check_boss_hierarchy },
    {"cheapest_friendpath", "([0-9A-Za-z]+)[[:space:]]+([0-9A-Za-z]+)", &cmd_cheapest_friendpath, &test_cheapest_friendpath },
    {"leave_cheapest_friendforest", "", &cmd_leave_cheapest_friendforest, &test_leave_cheapest_friendforest },
    {"quit", "", nullptr, nullptr },
    {"help", "", &help_command, nullptr },
    {"read", "'([-a-zA-Z ./:_]+)'", &cmd_read, nullptr },
    {"testread", "'([-a-zA-Z ./:_]+)'[[:space:]]+'([-a-zA-Z ./:_]+)'", &cmd_testread, nullptr },
    {"perftest", "([0-9a-zA-Z_]+(?:;[0-9a-zA-Z_]+)*)[[:space:]]+([0-9]+)[[:space:]]+([0-9]+)[[:space:]]+([0-9]+)[[:space:]]+([0-9]+(?:;[0-9]+)*)", &cmd_perftest, nullptr },
    {"stopwatch", "(?:(on)|(off)|(next))", &cmd_stopwatch, nullptr },
    {"random_seed", "([0-9]+)", &cmd_randseed, nullptr },
    {"#", ".*", [](Datastructure&, ostream&, MatchIter, MatchIter){}, nullptr },
};

void help_command(Datastructure& /*ds*/, ostream& output, MatchIter /*begin*/, MatchIter /*end*/)
{
    output << "Commands:" << endl;
    for (auto& i : cmds)
    {
        output << "  " << i.cmd << endl;
    }
}

void cmd_perftest(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
#ifdef _GLIBCXX_DEBUG
    output << "WARNING: Debug STL enabled, performance will be worse than expected (maybe also asymptotically)!" << endl;
#endif // _GLIBCXX_DEBUG

    vector<string> optional_cmds({"remove", "nearest_common_boss", "higher_lower_ranks", "shortest_friendpath", "check_boss_hierarchy", "cheapest_friendpath",
                                  "leave_cheapest_friendforest"});

    string commandstr = *begin++;
    unsigned int timeout = convert_string_to<unsigned int>(*begin++);
    unsigned int repeat_count = convert_string_to<unsigned int>(*begin++);
    unsigned int friend_count = convert_string_to<unsigned int>(*begin++);
    string sizes = *begin++;
    assert(begin == end && "Invalid number of parameters");

    vector<string> testcmds;
    if (commandstr != "all" && commandstr != "compulsory")
    {
        regex commands_regex("([0-9a-zA-Z_]+);?");
        smatch scmd;
        auto cbeg = commandstr.cbegin();
        auto cend = commandstr.cend();
        for ( ; regex_search(cbeg, cend, scmd, commands_regex); cbeg = scmd.suffix().first)
        {
            testcmds.push_back(scmd[1]);
        }
    }

    vector<unsigned int> init_ns;
    regex sizes_regex("([0-9]+);?");
    smatch size;
    auto sbeg = sizes.cbegin();
    auto send = sizes.cend();
    for ( ; regex_search(sbeg, send, size, sizes_regex); sbeg = size.suffix().first)
    {
        init_ns.push_back(convert_string_to<unsigned int>(size[1]));
    }

    output << "Timeout for each N is " << timeout << " sec. " << endl;
    output << "Add 0.." << friend_count << " friends for every employee." << endl;
    output << "For each N perform " << repeat_count << " random command(s) from:" << endl;

    // Initialize test functions
    vector<function<void(Datastructure& ds)>> testfuncs;
    if (testcmds.empty())
    { // Add all commands
        for (auto& i : cmds)
        {
            if (i.testfunc)
            {
                if (commandstr == "all" || find(optional_cmds.begin(), optional_cmds.end(), i.cmd) == optional_cmds.end())
                {
                    output << i.cmd << " ";
                    testfuncs.push_back(i.testfunc);
                }
            }
        }
    }
    else
    {
        for (auto& i : testcmds)
        {
            auto pos = find_if(cmds.begin(), cmds.end(), [&i](auto& cmd){ return cmd.cmd == i; });
            if (pos != cmds.end() && pos->testfunc)
            {
                output << i << " ";
                testfuncs.push_back(pos->testfunc);
            }
            else
            {
                output << "(cannot test " << i << ") ";
            }
        }
    }
    output << endl << endl;

    if (testfuncs.empty())
    {
        output << "No commands to test!" << endl;
        return;
    }

    output << setw(7) << "N" << " , " << setw(12) << "add (sec)" << " , " << setw(12) << "cmds (sec)"  << " , " << setw(12) << "total (sec)" << endl;

    for (unsigned int n : init_ns)
    {
        output << setw(7) << n << " , " << flush;

        ds.clear();
        init_primes();

        Stopwatch stopwatch;
        stopwatch.start();

        for (unsigned int i = 0; i < n / 1000; ++i)
        {
            add_random_persons(ds, 1000);

            if (stopwatch.elapsed() >= timeout) { break; }
        }
        add_random_persons(ds, n % 1000);

        for (unsigned int i = 0; i < people_added; ++i)
        {
            auto friends = random(0u, friend_count+1);
            while (friends != 0)
            {
                auto id1 = n_to_id(i);
                auto id2 = n_to_id(random<decltype(people_added)>(0, people_added));
                if (id1 != id2)
                {
                    auto cost = random<Cost>(0, 10);
                    ds.add_friendship(id1, id2, cost);
                }
                --friends;
            }
        }

        auto addsec = stopwatch.elapsed();
        output << setw(12) << addsec << " , " << flush;
        if (stopwatch.elapsed() >= timeout) { break; }

        for (unsigned int repeat = 0; repeat < repeat_count; ++repeat)
        {
            auto cmdpos = random(testfuncs.begin(), testfuncs.end());

            (*cmdpos)(ds);

            if (stopwatch.elapsed() >= timeout) { break; }
        }

        if (stopwatch.elapsed() >= timeout)
        {
            output << "Timeout!" << endl;
            break;
        }

        stopwatch.stop();
        auto totalsec = stopwatch.elapsed();
        output << setw(12) << totalsec-addsec << " , " << setw(12) << totalsec;

//        unsigned long int maxmem;
//        string unit;
//        tie(maxmem, unit) = mempeak();
//        maxmem -=  startmem;
//        if (maxmem != 0)
//        {
//            output << ", memory " << maxmem << " " << unit;
//        }
        output << endl;
    }

    ds.clear();
    init_primes();

#ifdef _GLIBCXX_DEBUG
    output << "WARNING: Debug STL enabled, performance will be worse than expected (maybe also asymptotically)!" << endl;
#endif // _GLIBCXX_DEBUG
}

void command_parser(Datastructure& ds, istream& input, ostream& output, PromptStyle promptstyle)
{
//    static unsigned int nesting_level = 0; // UGLY! Remember nesting level to print correct amount of >:s.
//    if (promptstyle != PromptStyle::NO_NESTING) { ++nesting_level; }

    // Create regex <whitespace>(cmd1|cmd2|...)<whitespace>(.*)
    string cmds_regex_str = "[[:space:]]*(";
    bool first = true;
    for (auto cmd : cmds)
    {
        cmds_regex_str += (first ? "" : "|") + cmd.cmd;
        first = false;
    }
    cmds_regex_str += ")(?:[[:space:]]*$|[[:space:]]+(.*))";
    regex cmds_regex(cmds_regex_str);

    string line;
    do
    {
//        output << string(nesting_level, '>') << " ";
        output << '>' << " ";
        getline(input, line, '\n');

        if (promptstyle != PromptStyle::NO_ECHO)
        {
            output << line << endl;
        }

        if (!input) { break; }

        smatch match;
        bool matched = regex_match(line, match, cmds_regex);
        if (matched)
        {
            assert(match.size() == 3);
            string cmd = match[1];
            string params = match[2];

            auto pos = find_if(cmds.begin(), cmds.end(), [cmd](CmdInfo const& ci) { return ci.cmd == cmd; });
            assert(pos != cmds.end());

            string params_regex_str = pos->param_regex_str+"[[:space:]]*";
            smatch match;
            bool matched = regex_match(params, match, regex(params_regex_str));
            if (matched)
            {
                if (pos->func)
                {
                    assert(!match.empty());

                    Stopwatch stopwatch;
                    bool use_stopwatch = (stopwatch_mode != StopwatchMode::OFF);
                    // Reset stopwatch mode if only for the next command
                    if (stopwatch_mode == StopwatchMode::NEXT) { stopwatch_mode = StopwatchMode::OFF; }

                    if (use_stopwatch)
                    {
                        stopwatch.start();
                    }

                    (pos->func)(ds, output, ++(match.begin()), match.end());

                    if (use_stopwatch)
                    {
                        stopwatch.stop();
                        output << "Command '" << cmd << "': " << stopwatch.elapsed() << " sec" << endl;
                    }
                }
                else
                { // No function to run = quit command
                    break;
                }
            }
            else
            {
                output << "Invalid parameters for command '" << cmd << "'!" << endl;
            }
        }
        else
        {
            output << "Unknown command!" << endl;
        }
    }
    while (input);

//    if (promptstyle != PromptStyle::NO_NESTING) { --nesting_level; }
}
} // namespace

int main(int argc, char* argv[])
{
    vector<string> args(argv, argv+argc);

    if (args.size() > 2)
    {
        cerr << "Usage: " + ((args.size() > 0) ? args[0] : "<program name>") + " [<command file>]" << endl;
        return EXIT_FAILURE;
    }

    rand_engine.seed(time(nullptr));
//    startmem = get<0>(mempeak());

    init_primes();

    {
        Datastructure ds;

        if (args.size() == 2)
        {
            string filename = args[1];
            ifstream input(filename);
            if (input)
            {
                command_parser(ds, input, cout, PromptStyle::NORMAL);
            }
            else
            {
                cout << "Cannot open file '" << filename << "'!" << endl;
            }
        }
        else
        {
            command_parser(ds, cin, cout, PromptStyle::NO_ECHO);
        }
        if (test_status != TestStatus::NOT_RUN)
        {
            cerr << "Tests have been run (testread), " << ((test_status == TestStatus::DIFFS_FOUND) ? "differences found!" : "no differences found.") << endl;
        }
    }

    cerr << "Program ended normally." << endl;
}
