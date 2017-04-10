// Definition of Datastructure class, hw3 of TIE-20100/TIE-20106

#include "datastructure.hpp"

// For debug
#include <iostream>
#include <ctype.h>
using std::cerr;
using std::endl;

// Helper function to return a random (enough) number in range start-end
// I.e. random_in_range(1,4) produces a random number between 1, 2, 3, or 4.
template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = rand() % range;
    return static_cast<Type>(start+num);
}

//---------------My Helper Functions for merge sort-------------------

void merge(vector<Person*>& listOfSalaries, int l, int m, int r, int sort_type)
{
    int  i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    /* Copy data to temp vector L and R */
    vector<Person*> L(listOfSalaries.begin() + l, listOfSalaries.begin() + n1 + l );
    vector<Person*> R(listOfSalaries.begin() + m + 1, listOfSalaries.begin() + m + n2 + 1);

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (sort_type == 0) {
            if ((*(L.at(i))).name <= (*(R.at(j))).name)
            {
                listOfSalaries.at(k) = L.at(i);
                i++;
            }
            else
            {
                listOfSalaries[k] = R[j];
                j++;
            }
            k++;
        } else if (sort_type == 1) {
            Person tmp = *(L.at(i));
            if ((*(L.at(i))).salary <= (*(R.at(j))).salary)
            {
                listOfSalaries.at(k) = L.at(i);
                i++;
            }
            else
            {
                listOfSalaries[k] = R[j];
                j++;
            }
            k++;
        }
    }

    /* Copy the remaining elements of L vector, if there
       are any */
    while (i < n1)
    {
        listOfSalaries[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R vector, if there
       are any */
    while (j < n2)
    {
        listOfSalaries[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(vector<Person*>& listOfSalaries, int l, int r,int sort_type)
{
    if (l < r)
    {
        int m = l+(r-l)/2;
        mergeSort(listOfSalaries, l, m, sort_type);
        mergeSort(listOfSalaries, m+1, r, sort_type);
        merge(listOfSalaries, l, m, r, sort_type);
    }
}

//--------------------------------------------------------------------

Datastructure::Datastructure()
{
    sorted_by_salary = 0;
}

Datastructure::~Datastructure()
{
    salary_list_.clear();
    sorted_by_salary = 0;
}

void Datastructure::add_person(std::string name, int salary)
{
    sorted_by_salary = 0;
    Person tmp_person;
    tmp_person.name = name;
    tmp_person.salary = salary;
    salary_list_.push_back(tmp_person);
//    vector<Person>::iterator iter = salary_list_.end();
//    salary_list_pointer_.push_back(&salary_list_.at(salary_list_.size()-1));
//    salary_list_pointer_.clear();
}

unsigned int Datastructure::size()
{
    return salary_list_.size();
}

void Datastructure::clear()
{
    salary_list_.clear();
    salary_list_pointer_.clear();
    sorted_by_salary = 0;
}

vector<Person *> Datastructure::personnel_alphabetically()
{
    int sort_type = 0;
    vector<Person*> salary_list_pointer;
    for (vector<Person>::iterator iter = salary_list_.begin(); iter != salary_list_.end(); ++iter)
        salary_list_pointer.push_back(&(*iter));
    mergeSort(salary_list_pointer, 0, salary_list_.size() - 1, sort_type);
    sorted_by_salary = 0;
    salary_list_pointer_ = salary_list_pointer;
    return salary_list_pointer_;
}

vector<Person *> Datastructure::personnel_salary_order()
{
    int sort_type = 1;
    vector<Person*> salary_list_pointer;
    for (vector<Person>::iterator iter = salary_list_.begin(); iter != salary_list_.end(); ++iter)
        salary_list_pointer.push_back(&(*iter));
    mergeSort(salary_list_pointer, 0, salary_list_.size() - 1, sort_type);
    sorted_by_salary = 1;
    salary_list_pointer_ = salary_list_pointer;
    return salary_list_pointer_;
}

Person* Datastructure::min_salary()
{
    if (sorted_by_salary == 1) {
        return *(salary_list_pointer_.begin());
    }
    else {
        vector<Person>::iterator min_iter;
        min_iter = salary_list_.begin();
        for (vector<Person>::iterator iter = salary_list_.begin(); iter != salary_list_.end(); ++iter)
            if ((*iter).salary < (*min_iter).salary) {
                min_iter = iter;
            }
        return &(*min_iter);
    }
}

Person* Datastructure::max_salary()
{
    if (sorted_by_salary == 1) {
        return salary_list_pointer_.back();
    }
    else {
        vector<Person>::iterator max_iter;
        max_iter = salary_list_.begin();
        for (vector<Person>::iterator iter = salary_list_.begin(); iter != salary_list_.end(); ++iter)
            if ((*iter).salary > (*max_iter).salary) {
                max_iter = iter;
            }
        return &(*max_iter);
    }
}

Person* Datastructure::median_salary()
{
    if (sorted_by_salary == 0) {
        this->personnel_salary_order();
    }
    return salary_list_pointer_[(salary_list_pointer_.size()) / 2];
}

Person* Datastructure::first_quartile_salary()
{
    if (sorted_by_salary == 0) {
        this->personnel_salary_order();
    }
    return salary_list_pointer_[(salary_list_pointer_.size()) / 4];
}

Person* Datastructure::third_quartile_salary()
{
    if (sorted_by_salary == 0) {
        this->personnel_salary_order();
    }
    return salary_list_pointer_[(3 * salary_list_pointer_.size()) / 4];
}
