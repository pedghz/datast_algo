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
using std::max_element;

#include <cstdlib>
using std::srand;
using std::rand;

#include <chrono>

#include <functional>
using std::function;
using std::equal_to;

#include <vector>
using std::vector;

#include <cstdlib>
#include <cassert>


namespace
{
//unsigned int startmem = 0;
using Salary = unsigned int;
using PersonList = vector<Person*>;

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

template <typename Type>
Type random(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = rand() % range;
    return static_cast<Type>(start+num);
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

void add_random_persons(Datastructure& ds, unsigned int size)
{
    for (unsigned int i = 0; i < size; ++i)
    {
        string name;
        for (unsigned int j = 0; j < 15; ++j)
        {
            name += random('a', 'z');
        }
        Salary salary = random<Salary>(1, 10*size);
        ds.add_person(name, salary);
    }
}

enum class PromptStyle { NORMAL, NO_ECHO, NO_NESTING };
void command_parser(Datastructure& ds, istream& input, ostream& output, PromptStyle promptstyle);

void help_command(Datastructure& ds, ostream& /*output*/, MatchIter /*begin*/, MatchIter /*end*/);

void cmd_add(Datastructure& ds, ostream& /*output*/, MatchIter begin, MatchIter end)
{
    string name = *begin++;
    string salarystr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    Salary salary = convert_string_to<Salary>(salarystr);

    ds.add_person(name, salary);
}

void cmd_random_add(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    string sizestr = *begin++;
    assert( begin == end && "Impossible number of parameters!");

    unsigned int size = convert_string_to<unsigned int>(sizestr);

    add_random_persons(ds, size);

    output << "Added: " << size << " persons" << endl;
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
            }
            else
            {
                output << "**Differences found! (Lines beginning with '?')**" << endl;
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

void cmd_perftest(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)
{
    unsigned int timeout = convert_string_to<unsigned int>(*begin++);
    unsigned int repeat_count = convert_string_to<unsigned int>(*begin++);
    string sizes = *begin++;
    assert(begin == end && "Invalid number of parameters");

    vector<unsigned int> init_ns;
    regex sizes_regex("([0-9]+);?");
    smatch size;
    auto sbeg = sizes.cbegin();
    auto send = sizes.cend();
    for ( ; regex_search(sbeg, send, size, sizes_regex); sbeg = size.suffix().first)
    {
        init_ns.push_back(convert_string_to<unsigned int>(size[1]));
    }

    output << "Timeout for each cycle is " << timeout << endl;
    output << "Repeat each cycle " << repeat_count << " times" << endl;

    for (unsigned int n : init_ns)
    {
        output << "n = " << n << " : " << flush;

        ds.clear();

        Stopwatch stopwatch;
        stopwatch.start();

        for (unsigned int i = 0; i < n / 1000; ++i)
        {
            add_random_persons(ds, 1000);

            if (stopwatch.elapsed() >= timeout) { break; }
        }
        add_random_persons(ds, n % 1000);

        if (stopwatch.elapsed() >= timeout) { break; }

        for (unsigned int repeat = 0; repeat < repeat_count; ++repeat)
        {
            enum CMDS { FIRST, ADD=FIRST, MIN, MAX, MEDIAN, QUARTILE1, QUARTILE3, SALARYLIST, ALPHALIST, LAST=ALPHALIST };
            auto cmd = random<CMDS>(FIRST, LAST);

            switch (cmd)
            {
            case ADD:
                add_random_persons(ds, 1);
                break;
            case MIN:
                ds.min_salary();
                break;
            case MAX:
                ds.max_salary();
                break;
            case MEDIAN:
                ds.median_salary();
                break;
            case QUARTILE1:
                ds.first_quartile_salary();
                break;
            case QUARTILE3:
                ds.third_quartile_salary();
                break;
            case SALARYLIST:
                ds.personnel_salary_order();
                break;
            case ALPHALIST:
                ds.personnel_alphabetically();
                break;
            default:
                assert(!"Impossible command in perfest!");
            }

            if (stopwatch.elapsed() >= timeout) { break; }
        }

        if (stopwatch.elapsed() >= timeout)
        {
            output << "Timeout!" << endl;
            break;
        }

        stopwatch.stop();
        output << stopwatch.elapsed() << " sec";

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

    output << "Cleared all persons" << endl;
}

void print_person(Person const* person, ostream& output)
{
    if (person)
    {
        output << person->name << ", salary " << person->salary;
    }
    else
    {
        output << "-- unknown --";
    }
}

void print_person_list(PersonList const& person_list, ostream& output)
{
    for (auto person : person_list)
    {
        print_person(person, output);
        output << endl;
    }
}

template<Person*(Datastructure::*MFUNC)()>
void NoParPersonCmd(Datastructure& ds, ostream& output, MatchIter /*begin*/, MatchIter /*end*/)
{
    auto result = (ds.*MFUNC)();
    print_person(result, output);
    output << endl;
}

template<PersonList(Datastructure::*MFUNC)()>
void NoParListCmd(Datastructure& ds, ostream& output, MatchIter /*begin*/, MatchIter /*end*/)
{
    auto result = (ds.*MFUNC)();
    print_person_list(result, output);
}

struct CmdInfo
{
    string cmd;
    string param_regex_str;
    function<void(Datastructure& ds, ostream& output, MatchIter begin, MatchIter end)> func;
};

vector<CmdInfo> cmds =
{
    {"add", "'([a-zA-Z ]+)'[[:space:]]+([0-9]+)", &cmd_add},
    {"random_add", "([0-9]+)", &cmd_random_add},
    {"read", "'([-a-zA-Z ./:_]+)'", &cmd_read},
    {"testread", "'([-a-zA-Z ./:_]+)'[[:space:]]+'([-a-zA-Z ./:_]+)'", &cmd_testread},
    {"size", "", &cmd_size},
    {"clear", "", &cmd_clear},
    {"alphalist", "", &NoParListCmd<&Datastructure::personnel_alphabetically>},
    {"salarylist", "", &NoParListCmd<&Datastructure::personnel_salary_order>},
    {"min", "", &NoParPersonCmd<&Datastructure::min_salary>},
    {"max", "", &NoParPersonCmd<&Datastructure::max_salary>},
    {"median", "", &NoParPersonCmd<&Datastructure::median_salary>},
    {"1stquartile", "", &NoParPersonCmd<&Datastructure::first_quartile_salary>},
    {"3rdquartile", "", &NoParPersonCmd<&Datastructure::third_quartile_salary>},
    {"quit", "", nullptr},
    {"help", "", &help_command},
    {"perftest", "([0-9]+)[[:space:]]+([0-9]+)[[:space:]]+([0-9]+(?:;[0-9]+)*)", &cmd_perftest},
    {"stopwatch", "(?:(on)|(off)|(next))", &cmd_stopwatch},
    {"#", ".*", [](Datastructure&, ostream&, MatchIter, MatchIter){} },
};

void help_command(Datastructure& /*ds*/, ostream& output, MatchIter /*begin*/, MatchIter /*end*/)
{
    output << "Commands:" << endl;
    for (auto& i : cmds)
    {
        output << "  " << i.cmd << endl;
    }
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

    srand(0);
//    startmem = get<0>(mempeak());

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
}
