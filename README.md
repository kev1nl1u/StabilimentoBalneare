# Stabilimento Balneare
- [Introduction](#introduction)
- [Software](#software)
	- [Files](#files)
	- [Usage](#usage)
- [Libraries](#libraries)
	- [Rink](#rink)
	- [Group](#group)
	- [SkatesStock](#skatesstock)
	- [Skate](#skate)
- [Project Status](#Project-Status)
- [Contacts](#Contacts)
- [License](#License)

# Introduction
This is a C++ software used to manage a beach resort.
> The current version is updated to manage an ice rink for the winter.

# Software
### Files
The libraries in the folder [/lib/](https://github.com/kev1nl1u/StabilimentoBalneare/tree/main/lib) contains the classes needed to run the resort.
In the main file [StabilimentoBalneare.cpp](https://github.com/kev1nl1u/StabilimentoBalneare/blob/main/StabilimentoBalneare.cpp) the resort is simulated in real time.
### Usage
Compile and run the main file [StabilimentoBalneare.cpp](https://github.com/kev1nl1u/StabilimentoBalneare/blob/main/StabilimentoBalneare.cpp) for a simulation of the resort.
It's possible to use the libraries with a customized main.

# Libraries
In the folder [/lib/](https://github.com/kev1nl1u/StabilimentoBalneare/tree/main/lib) there are the following libraries:
> All the classes (except `Group`) have a method to update the time to calculate the revenue and expenses. The method `updateTime(int time)` in the class `Rink` update the time of the other classes.
### Rink
The class `Rink` is used to manage the ice rink.<br>
It includes the management of the stocks of the skates with the integration of the library `SkateStock`. The library `Group` is needed to manage the people in the rink.<br>
The class has methods to:
- add a group of people, returns the number of people in the rink or -1 if the operation fails
```cpp
	int addGroup(Group group);
```
- remove a group of people with the index in the list of groups, returns the number of people in the rink
```cpp
	int removeGroup(int index);
```
- if a group can be added, returnes true or false
```cpp
	bool canAddGroup(Group group);
```
- calculate revenue and expenses
- the various getters
### Group
The class Group represent a group of people, one person only is considered a group as well.<br>
Each person in the group has a skates size.<br>
When the group enters the rink, it's saved the time of enter and to each person is assigned a skate with its ID.<br>
The class has the various getters and setters.<br>
### SkatesStock
The class SkatesStock is used to manage the skates stock, it includes all the skates.<br>
The class has methods to:
- remove stock for a group, used when a group enters the rink
```cpp
	void removeStockForGroup(Group g);
```
- add stock for a group, used when a group leaves the rink
```cpp
	void addStockForGroup(Group g);
```
- if a group can be added, returnes true or false
```cpp
	bool checkAvailabilityForGroup(Group g);
```
- calculate the expenses for the skates maintenance, it sums all the maintenance costs for every size of skates
- the various getters
- overload of the operator `<<` to print the skates stock
### Skate
The class Skate is used to manage the skates divided by size.<br>
The single skate is a struct with the following fields:
- `int id`: the ID of the skate
- `int rentalsLeft`: rentals left before maintenance
- `bool inMaintenance`: if the skate is in maintenance
- `bool available`: if the skate is available
- `int timeStartMaintenance`: the time when the maintenance started

The class saves all the skates in a vector of struct.<br>
Every skate has an ID, which is the position in the vector of skates of its size. The ID is given to a person in the group when it enters the rink.<br>
It has another vector for the available skates (public). When a skated is being used or it goes in maintenance, it's removed from the available skates vector.<br>
The class has methods to:
- use a skate: removes the skate from the available skates vector, decrease rentals left and returns the ID of the skate
```cpp
	int useSkate();
```
- return a skate: get the ID of the skate used by the people in the group that exited the rink, add the skate to the available skates vector and checks if the skate needs maintenance (rentals left = 0)
```cpp
	void deuseSkate(int index);
```
- check if a skate needs maintenance: called when a skate is returned
- check when a skate has finished maintenance: checked every time the time is updated
- the various getters and setters
- overload of the operator `<<` to print the status of the skates of that size
- copy constructor

# Project Status
The project is finished. It's possible to add new features or to improve the code.<br>
The project is open to contributions.

# Contacts
<img src="https://www.fermimn.edu.it/img/Logo_ITIS_MN2.png" style="height: 1em"> cliu@fermimn.edu.it

# License <img src="https://licensebuttons.net/l/by/3.0/88x31.png" style="height: 1em;">
<a href="https://creativecommons.org/licenses/by/4.0/">Creative Commons CC BY</a>
