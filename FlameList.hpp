#ifndef FLAME_LIST_HPP
#define FLAME_LIST_HPP

#include <string> //string based class
#include <sstream> //input & output string stream
#include <iostream> //input & output stream
#include <cassert> //assert.h, defines a standard debugging tool : assert().
#include <stdexcept> //this header defines a set of standard exceptions.
#include <exception> //this header defines several types and utilities to assist handling exceptions.

namespace flame {
    template<typename Item = int,typename Label = std::string>
    class MarkList {
    public:
        class MarkPoint {
            /* if data is transferred by point, it will be release at the end!*/
        public:
            MarkPoint(Item __data = Item()) {
                _dataPoint = new Item(__data);
                _prev = nullptr;
                _next = nullptr;
            }

            MarkPoint(Item __data,Label __label) {
                _dataPoint = new Item(__data);
                _label = __label;
                _prev = nullptr;
                _next = nullptr;
            }

            MarkPoint(const MarkPoint &node) {
                _label = node.label();
                _dataPoint = new Item(node.data());
                _prev = nullptr;
                _next = nullptr;
            }

            ~MarkPoint() {
                delete _dataPoint;
            }

            bool setData(Item data = Item()) {
                *_dataPoint = data;
            }

            Item data() {
                return *_dataPoint;
            }

            Item *dataPoint() {
                return _dataPoint;
            }

            void setLabel(Label _label){
                this->_label = _label;
            }

            Label label(){
                return this->_label;
            }

            MarkPoint * & prev(){
                return this->_prev;
            }

            MarkPoint * & next(){
                return this->_next;
            }

            friend std::ostream &operator<<(std::ostream &cout, MarkPoint &item) {
                cout << "data : " << item.data() << "label : " << item.label();
                return cout;
            }

            friend std::ostream &operator<<(std::ostream &cout, MarkPoint *item) {
                cout << "data : " << item->data() << "label : " << item->label();
                return cout;
            }

            std::string toStr(){
                std::stringstream stream;
                std::string datastr;
                std::string labelstr;
                stream << "data:";
                stream << this->data();
                stream >> datastr;
                stream.clear();
                stream << "label:";
                stream << this->label();
                stream >> labelstr;
                return datastr + "\t\t" + labelstr + "\t";
            }

            std::string data2Str(){
                std::stringstream stream;
                std::string datastr;
                stream << this->data();
                stream >> datastr;
                return datastr;
            }

            std::string label2Str(){
                std::stringstream stream;
                std::string labelstr;
                stream << this->label();
                stream >> labelstr;
                return labelstr;
            }
        private:
            MarkPoint *_prev;
            MarkPoint *_next;
            Label _label;
            Item *_dataPoint = nullptr;
        };

        MarkList() {
            _size = 0;
            head = new MarkPoint(Item());
            trail = new MarkPoint(Item());
            head->next() = trail;
            head->prev() = trail;
            trail->prev() = head;
            trail->next() = head;
        }

        ~MarkList() {
            while (head->next() != trail) {
                delete this->pop_back();
            }
            delete head;
            delete trail;
        }

        bool isEmpty() {
            return _size == 0;
        }

        unsigned long long int size() {
            return _size;
        }

        bool append(Item item,Label label) {
            return this->append(new MarkPoint(item,label));
        }

        bool push_back(Item item) {
            return this->append(new MarkPoint(item));
        }

        bool insert(Item item, unsigned long long int n) {
            return this->insert(new MarkPoint(item), n);
        }

        bool push_front(Item item) {
            return this->insert(new MarkPoint(item), 0);
        }

        MarkPoint *pop_front() {
            if (head->next() != trail) {
                MarkPoint *item = head->next();
                head->next() = item->next();
                item->next()->prev() = head;
                item->next() = nullptr;
                item->prev() = nullptr;
                _size--;
                return item;
            } else throw (std::out_of_range("This list is empty!"));
        }

        MarkPoint *pop_back() {
            if (head != trail->prev()) {
                MarkPoint *item = trail->prev();
                trail->prev() = item->prev();
                item->prev()->next() = trail;
                item->next() = nullptr;
                item->prev() = nullptr;
                _size--;
                return item;
            } else throw (std::out_of_range("This list is empty!"));
        }


        MarkPoint *begin() {
            if (head->next() != trail)
                return head->next();
            else throw (std::out_of_range("This list is empty!"));
        }

        MarkPoint *end() {
            if (head != trail->prev())
                return trail->prev();
            else throw (std::out_of_range("This list is empty!"));
        }

        void clear() {
            while (head->next() != trail) {
                delete this->pop_back();
            }
        }

        MarkPoint *operator[](unsigned long long int n) {
            return getItem(n);
        }

        friend std::ostream &operator<<(std::ostream &cout, MarkList &list) {
            MarkPoint *temp = list.head->next();
            while (temp != list.trail) {
                cout << temp;
                temp = temp->next();
                if (temp != list.trail)
                    cout << " ";
            }
            return cout;
        }

        friend std::ostream &operator<<(std::ostream &cout, MarkList *list) {
            MarkPoint *temp = list->head->next();
            while (temp != list->trail) {
                cout << temp;
                temp = temp->next();
                if (temp != list->trail)
                    cout << " ";
            }
            return cout;
        }

        std::string toStr(){
            std::string str;
            MarkPoint *temp = this->head->next();
            while (temp != this->trail) {
                str += temp->toStr();
                temp = temp->next();
                if (temp != this->trail)
                    str += "\n";
            }
            return str;
        }

        std::string toTable(){
            std::string str;
            str += "|";
            for (MarkPoint * node = this->begin();node != this->end();node=node->next()) {
                str += node->data2Str();
                str += "\t\t";
            }
            str += this->end()->data2Str();
            int addlength = this->end()->data2Str().length();
            str += "|\n|\t";
            for (MarkPoint * node = this->begin();node != this->end()->prev();node=node->next()) {
                str += node->label2Str();
                str += "\t|\t";
            }
            str += this->end()->prev()->label2Str();
            str += "\t";
            for (int i = 0;i<addlength;i++) {
                str +="  ";
            }
            str += "|";
            return str;
        }

        void initendPos(){
            endPos = this->end();
        }

        MarkPoint * getCurrent(){
            return endPos;
        }

        void setCurrentLabel(Label label){
            this->endPos->prev()->setLabel(label);
        }

        bool delPos(Item position){
            auto start = endPos;
            while(start!=trail && start->data() < position){
                start = start->next();
            }
            while(start!=head && start->data() > position){
                start = start->prev();
            }
            if(start==trail || start==this->begin() || start==this->end() || start==head) {
                return false;
            }
            if(start->data()==position){
                if(endPos==start)
                    endPos = start->next();
                start->prev()->next() = start->next();
                start->next()->prev() = start->prev();
                delete start;
                start = nullptr;
                _size--;
                return true;
            }else{
                return false;
            }
        }

        bool insertPos(Item position,Label label){
            auto start = endPos;
            while(start!=trail && start->data() < position){
                start = start->next();
            }
            while(start!=head && start->prev()->data() > position){
                start = start->prev();
            }
            if(start==trail||start==this->begin()||start==head) return false;
            MarkPoint *item = new MarkPoint(position,label);
            item->next() = start;
            item->prev() = start->prev();
            item->prev()->next() = item;
            item->next()->prev() = item;
            _size++;
            return true;
        }

        bool isChangeTag(Item position){
            bool change = false;
            while(endPos!=trail && endPos->data() < position){
                endPos = endPos->next();
                change = true;
            }
            while(endPos!=head && endPos->prev()->data() > position){
                endPos = endPos->prev();
                change = true;
            }
            return change;
        }

        int getTag(){
            return endPos->prev()->label();
        }

    private:
        bool append(MarkPoint *item) {
            _size++;
            item->prev() = trail->prev();
            item->prev()->next() = item;
            item->next() = trail;
            trail->prev() = item;
            return true;
        }

        bool insert(MarkPoint *item, unsigned long long int n) {
            if (_size >= n) {
                _size++;
                MarkPoint *temp = head;
                for (int i = 0; i < n; i++) {
                    temp = temp->next();
                }
                item->next() = temp->next();
                item->prev() = temp;
                item->prev()->next() = item;
                item->next()->prev() = item;
                return true;
            }
            return false;
        }

        bool remove(MarkPoint *item) {
            --_size;
            item->prev()->next() = item->next();
            item->next()->prev() = item->prev();
            delete item;
            item = nullptr;
            return true;
        }

        long long int find(MarkPoint *item) {
            long long int n = 0;
            if (item != nullptr) {
                MarkPoint *temp = head->next();
                while (temp != trail) {
                    if (temp != item)
                        temp = temp->next();
                    else return n;
                    n++;
                }
            }
            return -1;
        }

        MarkPoint *getItem(unsigned long long int n) {
            if (n > _size - 1 || n < 0) {
                std::stringstream ssTemp;
                ssTemp << n;
                throw (std::out_of_range("the index " + ssTemp.str() + " is out of range!"));
            }
            if (n < _size / 2) {
                MarkPoint *temp = head->next();
                for (unsigned long long int i = 0; i < n; i++) {
                    temp = temp->next();
                }
                return temp;
            } else {
                MarkPoint *temp = trail->prev();
                for (unsigned long long int i = 0; i < n; i++) {
                    temp = temp->prev();
                }
                return temp;
            }

        }

        void deleteItem(MarkPoint *start, MarkPoint *end) {
            auto temp = start;
            while (temp != end) {
                temp = temp->next();
                delete temp->prev();
            }
            delete end;
        }

        unsigned long long int _size;
        MarkPoint * head;
        MarkPoint * trail;
        MarkPoint * endPos;
    };
}

#endif // FLAME_LIST_HPP
