#include<iostream>
#include<string>
#include<fstream>
#include<chrono>
#include<thread>
#include<filesystem>
using namespace std;
using namespace std::filesystem;

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

// --- Helper Functions ---

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

int maxItemName(player &p) {
    int x=0;
    int n= p.totalItem;
    for(int i=0; i<n; i++) {
        int length = p.inventory[i].name.length();
        if(x<length) {
            x = length;
        }
    }

    if(x<9) {
        x=9;
    }
    
    return x;
}

int idx(player &p, int indx, int op) {
    if(op == 1) {
        return p.inventory[indx].type;
    } else if(op == 2) {
        return p.inventory[indx].rarity;
    } else {
        return p.inventory[indx].amount;
    }
}

int partition(player &p, int low, int high, int op, int typ) {
    int pivot = idx(p, high, op);
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        bool asc;
        if(typ == 1) {
            asc = idx(p, j, op) < pivot;
        } else {
            asc = idx(p, j, op) > pivot;
        }
        if (asc) {
            i++;
            item temp = p.inventory[i];
            p.inventory[i] = p.inventory[j];
            p.inventory[j] = temp;
        }
    }
    item temp = p.inventory[i + 1];
    p.inventory[i + 1] = p.inventory[high];
    p.inventory[high] = temp;
    return (i + 1);
}

void quickSort(player &p, int low, int high, int op, int typ) {
    if (low < high) {
        int pi = partition(p, low, high, op, typ);
        quickSort(p, low, pi - 1, op, typ);
        quickSort(p, pi + 1, high, op, typ);
    }
}

// --- File Management ---

void initDirectory() {
    if (!exists("save")) {
        create_directory("save");
        cout << "System: Folder 'save' created." << endl;
    }
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
    getline(cin, p.name);
    p.totalItem=0;
    p.inventory=nullptr;

    string slot = EmptySlot();

    save(p, slot);
    rnSlot = stoi(slot);
    addSlot(slot);
}

bool load(player &p,int &rnSlot) {
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
                cin.ignore();
                getline(file,p.name);
                file >> totalitem;
                p.totalItem = totalitem;
                file.ignore();
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

// --- Inventory Logic ---

void showInventory(player &p) {
    cout << "\n===" << p.name << "'s inventory===" << endl;
    cout << "Total items: " << p.totalItem << endl;
    
    int colNo = max(2, digits(p.totalItem));
    int colName = maxItemName(p);
    
    // Header Border
    cout << "+-"; coout('-', colNo); cout << "-+-"; coout('-', colName); cout << "-+--------+-----------+--------+" << endl;
    cout << "| No"; coout(' ', colNo - 2); cout << " | Item Name"; coout(' ', colName - 9); cout << " | Type   | Rarity    | Amount |" << endl;
    cout << "+-"; coout('-', colNo); cout << "-+-"; coout('-', colName); cout << "-+--------+-----------+--------+" << endl;

    if (p.totalItem == 0) {
        cout << "| - "; coout(' ', colNo - 2); cout << " | EMPTY"; coout(' ', colName - 5); cout << " | -      | -         | -      |" << endl;
    } else {
        for (int i = 0; i < p.totalItem; i++) {
            cout << "| " << i + 1; coout(' ', colNo - digits(i + 1));
            cout << " | " << p.inventory[i].name; coout(' ', colName - p.inventory[i].name.length());
            cout << " | " << type[p.inventory[i].type]; coout(' ', 6 - type[p.inventory[i].type].length());
            cout << " | " << rarity[p.inventory[i].rarity] << " | " << p.inventory[i].amount; coout(' ', 6 - digits(p.inventory[i].amount));
            cout << " |" << endl;
        }
    }
    cout << "+-"; coout('-', colNo); cout << "-+-"; coout('-', colName); cout << "-+--------+-----------+--------+" << endl;
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

void sortItem(player &p) {
    cout << "\nSorting Option" << endl;
    cout << "1. By Type" << endl;
    cout << "2. By Rarity" << endl;
    cout << "3. By Amount" << endl;
    cout << "Option : ";
    int op; cin >> op;

    cout << "Sorting Type" << endl;
    cout << "1. Ascendeing" << endl;
    cout << "2. Descending" << endl;
    cout << "Type : ";
    int typ; cin >> typ;

    quickSort(p, 0, p.totalItem-1, op, typ);
}

void discardItem(player &p) {
    if (p.totalItem == 0) {
        cout << "Nothing to discard." << endl;
        return;
    }
    cout << "Enter item number to discard (1-" << p.totalItem << "): ";
    int index; cin >> index;
    if (index < 1 || index > p.totalItem) {
        cout << "Invalid index!" << endl;
        return;
    }
    
    item* newInventory = (p.totalItem > 1) ? new item[p.totalItem - 1] : nullptr;
    int current = 0;
    for (int i = 0; i < p.totalItem; i++) {
        if (i == index - 1) continue;
        newInventory[current++] = p.inventory[i];
    }
    
    delete[] p.inventory;
    p.inventory = newInventory;
    p.totalItem--;
    cout << "Item discarded." << endl;
}

void advancedSearch(player &p) {
    if (p.totalItem == 0) {
        cout << "Inventory is empty!" << endl;
        return;
    }
    cout << "\n=== Advanced Search ===\n1. Name\n2. Type\n3. Rarity\nChoice: ";
    int choice; cin >> choice;
    bool found = false;


    if (choice == 1) {
        string key; cout << "Keyword: "; cin.ignore(); getline(cin, key);
        for (int i = 0; i < p.totalItem; i++) {
            if (p.inventory[i].name.find(key) != string::npos) {
                cout << "- (No: " << i+1 << ") "<< p.inventory[i].name << " (x" << p.inventory[i].amount << ")" << endl;
                found = true;
            }
        }
    } else if (choice == 2) {
        int t; cout << "1.Weapon 2.Armour 3.Potion: "; cin >> t;
        for (int i = 0; i < p.totalItem; i++) {
            if (p.inventory[i].type == t - 1) {
                cout << "-(No: " << i+1 << ") "<< p.inventory[i].name << " (x" << p.inventory[i].amount << ")" << endl;
                found = true;
            }
        }
    } else if (choice == 3) {
        int r; cout << "1.Common 2.Rare 3.Epic 4.Legendary: "; cin >> r;
        for (int i = 0; i < p.totalItem; i++) {
            if (p.inventory[i].rarity == r - 1) {
                cout << "- (No: " << i+1 << ") "<< p.inventory[i].name << " (x" << p.inventory[i].amount << ")" << endl;
                found = true;
            }
        }
    }
    if (!found) cout << "No matches found." << endl;
}

// --- Game Flow ---

void ingame(player &p, item &it, int &rnSlot) {
    cout << "Entering the world";
    for(int i=0; i<3; i++) {
        this_thread :: sleep_for(chrono::seconds(1));
        cout << ".";
    }
    cout << endl << endl;
    bool exit = false;
    while(!exit) {
        showInventory(p);
        cout << "Action : " << endl;
        cout << "1.Add item" << endl;
        cout << "2.Discard item" << endl;
        cout << "3.Sort inventory" << endl;
        cout << "4.Advance search" << endl;
        cout << "5.Save progres" << endl;
        cout << "6.Exit to main menu" << endl;
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
                discardItem(p);
                break;
            }
            case 3 : {
                sortItem(p);
                break;
            }
            case 4 : {
                advancedSearch(p);
                break;
            }
            case 5 : {
                save(p, to_string(rnSlot));
                cout << "Saving"; 
                for(int i=0; i<3; i++) {
                    this_thread :: sleep_for(chrono::seconds(1)); 
                    cout << ".";
                }
                cout << "\nGame saved!\n";
                this_thread :: sleep_for(chrono::seconds(2));
                break;
            }
            case 6 : {
                if(yesn("All unsaved progres will be lost, you sure want to exit? (y/n) : ")) {
                    exit = true;
                } else {
                    cout << endl;
                    continue;
                }
                break;
            }
            default : {
                cout << "Invalid Input!\n";
                break;
            }
        }
    }
    
}

void mainMenu(player &p, item &i, int &rnSlot) {
    bool exit = false;
    while(!exit) {
        cout << "===Welcome to Ngawi RPG game===" << endl;
        cout << "+-----------------------------+" << endl;
        cout << "|=main menu=                  |" << endl;
        cout << "|1.New Game                   |" << endl;
        cout << "|2.Load Game                  |" << endl;
        cout << "|3.Exit Game                  |" << endl;
        cout << "+-----------------------------+" << endl;
        bool val = false;
        while(!val && !exit) {
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
                    if(load(p, rnSlot)) {
                        ingame(p,i, rnSlot);
                        val = true;
                    };
                    break;
                }
                case 3 : {
                    if(yesn("You sure want to exit? (y/n) : ")) {
                        exit = true;
                        cout << "Byeee!\n";
                    } else {
                        continue;
                    }
                    break;
                }
                default : {
                    cout << "Invalid Input!\n";
                    break;
                }
            }
        }
    
    }
}

int main() {
    initDirectory();
    player p;
    p.inventory = nullptr;
    item i;
    int rnSlot;
    mainMenu(p, i, rnSlot);
}
