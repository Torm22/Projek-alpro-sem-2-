#include<iostream>
#include<string>
#include<fstream>
#include<chrono>
#include<thread>
using namespace std;

string rarity[] ={"Common   ", "Rare     ", "Epic     ", "Legendary"};
string type[] ={"Weapon", "Armour", "Potion"};

struct item{
    string name;
    int type;
    int rarity;
    int amount;
};

struct player{
    string name;
    item* inventory;
    int totalItem;
};

bool yesn(string command) {
    char yn;
    cout << command;
    cin >> yn;
    if(tolower(yn)=='y') {
        return true;
    } else if(tolower(yn)=='n') {
        return false;
    } else {
        cout << "Input Error, try again!"<< endl;
        return yesn(command);
    }
}

int digits(int x) {
    int ans = 0;
    if(x==0) {
        return 1;
    }
    while(x>0) {
        ans++;
        x = x/10;
    }
    return ans;
}

void coout(char c, int x) {
    if(x < 0) {
        x=0;
    }
    for(int i=0; i<x; i++) {
        cout << c;
    }
}

int maxItemName(player &p, item &i) {
    int x=0;
    int n= p.totalItem;
    for(int i=0; i<n; i++) {
        int length = p.inventory[i].name.length();
        if(x<length) {
            x = length;
        }
    }
    return x;
}

string EmptySlot() {
    ifstream file("save/slot.txt");
    string line;
    string lastLine;

    while(getline(file,line)) {
        lastLine = line;
    }
    file.close();

    string empty;

    if(lastLine=="") {
        lastLine = "0";
    }

    empty = to_string(stoi(lastLine)+1);
    return empty;
}

void addSlot(string slot) {
    ofstream file("save/slot.txt", ios::app);
    file << slot << endl;
    file.close();
}

void save(player &p, string slot) {
    ofstream file("save/save" + slot + ".txt");
    file << p.name << endl;
    file << p.totalItem << endl;

    for(int i=0; i<p.totalItem; i++) {
        file << p.inventory[i].name << "|";
        file << p.inventory[i].type << "|";
        file << p.inventory[i].rarity << "|";
        file << p.inventory[i].amount << endl;
    }

    file.close();
}

void createGame(player &p, int &rnSlot) {
    cout << "Player name : ";
    cin.ignore();
    getline(cin, p.name);\
    p.totalItem=0;
    p.inventory=nullptr;

    string slot = EmptySlot();

    save(p, slot);
    rnSlot = stoi(slot);
    addSlot(slot);
}

bool load(player &p, item &i, int &rnSlot) {
    if(EmptySlot()=="1") {
        cout << "Save file is empty, create New Game to make a save file!" << endl;
        return false;
    } else {
        cout << endl << "Save file(s) :" << endl;
        int nSave = stoi(EmptySlot())-1;
        for(int i=0; i<nSave; i++) {
            ifstream file("save/save" + to_string(i+1) + ".txt");
            string name;
            int totalItem;
            getline(file,name);
            //file.ignore();
            file >> totalItem;
            file.close();

            cout << i+1 << ". Name : " << name << " || Total Items : " << totalItem << endl;
        }
        bool cslot=false;

        while(!cslot) {
            cout << "Chose save file by slot : ";
            int x; cin >> x;

            ifstream file("save/save"+to_string(x)+".txt");
            if(!file) {
                cout << "Save file not found!, please enter the correct slot" << endl;
            } else {
                int totalitem;
                delete[] p.inventory;
                getline(file,p.name);
                //file.ignore();
                file >> totalitem;
                p.totalItem = totalitem;
                p.inventory = new item[p.totalItem];

                for(int i=0; i<p.totalItem; i++) {
                    string tmp;
                    getline(file, p.inventory[i].name, '|');
                    getline(file, tmp, '|'); p.inventory[i].type = stoi(tmp);
                    getline(file, tmp, '|'); p.inventory[i].rarity = stoi(tmp);
                    getline(file, tmp); p.inventory[i].amount = stoi(tmp);
                }

                file.close();
                cslot = true;
            }
            rnSlot = x;
        }
        cout << endl;
        cout << "You will play as " << p.name << endl;
        cout << "With total item : " << p.totalItem << endl;
        return true;
    }
}

void showInventory(player &p, item &it) {
    cout << "===" << p.name <<"'s inventory===" << endl;
    cout << "Total item : " << p.totalItem << endl;
    int colNo=digits(p.totalItem)-2, colName=maxItemName(p,it)-9;
    cout << "+=="; coout('=', colNo); cout << "=========="; coout('=',colName); cout << "========================+" << endl;
    cout << "|No"; coout(' ', colNo); cout << "|Item Name"; coout(' ',colName); cout << "|Type  |Rarity   |Amount|" << endl;
    if(p.totalItem==0) {
        cout << "|- |-        |-     |-        |-     |" << endl;
    } else {
        for(int i=0; i<p.totalItem; i++) {
            cout << "|" << i+1; coout(' ', max(1,digits(p.totalItem)-digits(i+1))); cout << "|" << p.inventory[i].name; coout(' ', maxItemName(p,it)-p.inventory[i].name.length()); cout <<"|" << type[p.inventory[i].type] << "|" << rarity[p.inventory[i].rarity] << "|" << p.inventory[i].amount; coout(' ', 6-digits(p.inventory[i].amount)); cout << "|";
            cout << endl;
        }
    }
    cout << "+=="; coout('=', colNo); cout << "=========="; coout('=',colName); cout << "========================+" << endl;
}

void addItem(player &p, string name, int type, int rar, int amount) {
    item* newInventory = new item[p.totalItem+1];

    for(int i=0; i<p.totalItem; i++) {
        newInventory[i] = p.inventory[i];
    }

    newInventory[p.totalItem].name = name;
    newInventory[p.totalItem].type = type-1;
    newInventory[p.totalItem].rarity = rar-1;
    newInventory[p.totalItem].amount = amount;

    delete[] p.inventory;
    p.inventory = newInventory;
    p.totalItem++;

    cout << "Item " << name << " successfully added to inventory" << endl << endl;
}

void ingame(player &p, item &it, int &rnSlot) {
    cout << "Entering the world";
    for(int i=0; i<3; i++) {
        this_thread :: sleep_for(chrono::seconds(1));
        cout << ".";
    }
    cout << endl << endl;
    bool exit = false;
    while(!exit) {
        showInventory(p, it);
        cout << "Action : " << endl;
        cout << "1.Add item" << endl;
        cout << "2.Discard item" << endl;
        cout << "3.Sort inventory" << endl;
        cout << "4.Save progres" << endl;
        cout << "5.Exit to main menu" << endl;
        cout << "Chose Action (1-5): ";
        int act; cin >> act;

        switch(act) {
            case 1 : {
                string name;
                int typ;
                int rar;
                int amount;

                cin.ignore();
                cout << "Name : "; getline(cin, name);
                cout << "Type : \n1.Weapon\n2.Armour\n3.Potion\nInsert (1-3) : "; cin >> typ;
                cout << "Rarity : \n1.Common\n2.Rare\n3.Epic\n4.Legendary\nInsert (1-4) : "; cin >> rar;
                cout << "Amount : "; cin >> amount;
                cout << endl;
    
                cout << "Name   : " << name << endl;
                cout << "Type   : " << type[typ-1] << endl;
                cout << "Rarity : " << rarity[rar-1] << endl;
                cout << "Amount : " << amount << endl;
                if(yesn("Add this item? (y/n) : ")) {
                    addItem(p, name, typ, rar, amount);
                } else {
                    cout << endl;
                    continue;
                }
                break;
            }
            case 2 : {

                break;
            }
        }
    }
    
}

void mainMenu(player &p, item &i, int &rnSlot) {
    cout << "===Welcome to Ngawi RPG game===" << endl;
    cout << "+—————————————————————————————+" << endl;
    cout << "|=main menu=                  |" << endl;
    cout << "|1.New Game                   |" << endl;
    cout << "|2.Load Game                  |" << endl;
    cout << "|3.Exit Game                  |" << endl;
    cout << "+—————————————————————————————+" << endl;
    bool val = false;
    while(!val) {
        cout << "Chose menu (1-3): ";
        int opp; cin >> opp;
        switch(opp) {
            case 1 : {
                createGame(p, rnSlot);
                ingame(p,i, rnSlot);
                val = true;
                break;
            }
            case 2 : {
                if(load(p,i, rnSlot)) {
                    ingame(p,i, rnSlot);
                    val = true;
                };
                break;
            }
            case 3 : {
                cout << "exit";
                break;
            }
            default : {
                cout << "input salah";
                break;
            }
        }
    }
}

int main() {
    player p;
    p.inventory = nullptr;
    item i;
    int rnSlot;
    mainMenu(p, i, rnSlot);
}