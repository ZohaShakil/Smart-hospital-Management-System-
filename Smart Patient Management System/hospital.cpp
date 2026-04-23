#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <ctime>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

struct Patient {
    int id;
    string name;
    int priority;
    time_t arrivalTime;
    time_t treatmentTime;

    Patient() : id(0), name(""), priority(1),
                arrivalTime(time(nullptr)), treatmentTime(0) {}

    Patient(int i, const string& n, int p)
        : id(i), name(n), priority(p),
          arrivalTime(time(nullptr)), treatmentTime(0) {}
};

/* ---------------- PRIORITY QUEUE ---------------- */

class PriorityQueue {
    vector<Patient> heap;

    void heapifyUp(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (heap[i].priority > heap[p].priority) {
                swap(heap[i], heap[p]);
                i = p;
            } else break;
        }
    }

    void heapifyDown(int i) {
        int n = heap.size();
        while (true) {
            int l = 2 * i + 1, r = 2 * i + 2, maxIdx = i;
            if (l < n && heap[l].priority > heap[maxIdx].priority) maxIdx = l;
            if (r < n && heap[r].priority > heap[maxIdx].priority) maxIdx = r;
            if (maxIdx == i) break;
            swap(heap[i], heap[maxIdx]);
            i = maxIdx;
        }
    }

public:
    void insert(const Patient& p) {
        heap.push_back(p);
        heapifyUp(heap.size() - 1);
    }

    Patient extractMax() {
        if (heap.empty()) throw runtime_error("Emergency queue empty");
        Patient top = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) heapifyDown(0);
        return top;
    }

    bool isEmpty() const { return heap.empty(); }
    int size() const { return heap.size(); }
};

/* ---------------- NORMAL QUEUE ---------------- */

class NormalQueue {
    struct Node {
        Patient data;
        Node* next;
        Node(const Patient& p) : data(p), next(nullptr) {}
    };
    Node* front;
    Node* rear;

public:
    NormalQueue() : front(nullptr), rear(nullptr) {}

    void enqueue(const Patient& p) {
        Node* n = new Node(p);
        if (!rear) front = rear = n;
        else { rear->next = n; rear = n; }
    }

    Patient dequeue() {
        if (!front) throw runtime_error("Normal queue empty");
        Node* t = front;
        Patient p = t->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete t;
        return p;
    }

    bool isEmpty() const { return front == nullptr; }
    int size() const {
        int c = 0;
        for (Node* cur = front; cur; cur = cur->next) c++;
        return c;
    }
};

/* ---------------- HASH TABLE ---------------- */

class HashTable {
    vector<list<Patient>> table;
    int sizeT;

    int hashFunc(int id) const { return id % sizeT; }

public:
    HashTable(int s = 211) : sizeT(s), table(s) {}

    void insert(const Patient& p) {
        int idx = hashFunc(p.id);
        for (auto& x : table[idx]) {
            if (x.id == p.id) { x = p; return; }
        }
        table[idx].push_back(p);
    }

    bool contains(int id) const {
        int idx = hashFunc(id);
        for (auto& x : table[idx]) if (x.id == id) return true;
        return false;
    }

    Patient search(int id) const {
        int idx = hashFunc(id);
        for (auto& x : table[idx]) if (x.id == id) return x;
        throw runtime_error("Patient not found");
    }

    void remove(int id) {
        int idx = hashFunc(id);
        table[idx].remove_if([id](Patient p) { return p.id == id; });
    }
};

/* ---------------- HOSPITAL SYSTEM ---------------- */

class HospitalSystem {
    PriorityQueue emerg;
    NormalQueue normal;
    HashTable records;
    vector<Patient> allPatients;

public:
    /* ---------- CSV LOAD ---------- */
    void loadCSV(const string& file) {
        ifstream f(file);
        if (!f) return;

        string line;
        getline(f, line); // header

        while (getline(f, line)) {
            stringstream ss(line);
            string temp;
            Patient p;

            getline(ss, temp, ','); p.id = stoi(temp);
            getline(ss, p.name, ',');
            getline(ss, temp, ','); p.priority = stoi(temp);
            getline(ss, temp, ','); p.arrivalTime = stoll(temp);
            getline(ss, temp, ','); p.treatmentTime = stoll(temp);

            records.insert(p);
            allPatients.push_back(p);

            if (p.priority > 1) emerg.insert(p);
            else normal.enqueue(p);
        }
        f.close();
    }

    /* ---------- CSV SAVE ---------- */
    void saveCSV(const string& file) {
        ofstream f(file);
        f << "id,name,priority,arrivalTime,treatmentTime\n";
        for (auto& p : allPatients) {
            f << p.id << "," << p.name << "," << p.priority << ","
              << p.arrivalTime << "," << p.treatmentTime << "\n";
        }
        f.close();
    }

    /* ---------- ADD PATIENT ---------- */
    void addPatient() {
        int id, priority;
        string name;
        char type;

        cout << "ID: "; cin >> id;
        cin.ignore();
        cout << "Name: "; getline(cin, name);
        cout << "Emergency? (y/n): "; cin >> type;

        priority = (type == 'y' || type == 'Y') ? 5 : 1;

        Patient p(id, name, priority);
        records.insert(p);
        allPatients.push_back(p);

        if (priority > 1) emerg.insert(p);
        else normal.enqueue(p);

        saveCSV("Hospital_Patients.csv");
        cout << "Patient added successfully.\n";
    }

    /* ---------- TREAT PATIENT ---------- */
    void treatPatient() {
        Patient p;
        if (!emerg.isEmpty()) p = emerg.extractMax();
        else if (!normal.isEmpty()) p = normal.dequeue();
        else { cout << "No patients.\n"; return; }

        p.treatmentTime = time(nullptr);
        records.insert(p);

        for (auto& x : allPatients)
            if (x.id == p.id) x.treatmentTime = p.treatmentTime;

        saveCSV("Hospital_Patients.csv");
        cout << "Patient treated: " << p.name << endl;
    }
};

/* ---------------- MAIN ---------------- */

int main() {
    HospitalSystem hs;
    hs.loadCSV("Hospital_Patients.csv");

    while (true) {
        cout << "\n1.Add Patient\n2.Treat Patient\n3.Exit\nChoice: ";
        int ch; cin >> ch;
        if (ch == 1) hs.addPatient();
        else if (ch == 2) hs.treatPatient();
        else break;
    }
    return 0;
}
