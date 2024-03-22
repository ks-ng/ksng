#include "../util/dstruct/sda.h"
#include "../util/dstruct/sll.h"
#include <cmath>
#include <random>

namespace nyx_1 {

    namespace internal {

        class Entity: public sda::SecureDataArray<double> {

            public:

                // Initializers

                Entity() {}

                Entity(int length_, bool locked_=false, Severity severityLevel_=ALERT) {
                    initialize(length_, locked_, severityLevel_);
                }

                Entity(initializer_list<double> initList) {
                    length = initList.size();
                    elements = new double[length];
                    locked = false;
                    securityLevel = ALERT;
                    int i = 0;
                    for (const auto& val : initList) {
                        elements[i++] = val;
                    }
                }

        };

        double distance(Entity a, Entity b) {
            double s = 0;
            for (int i = 0; i < a.getLength() && i < b.getLength(); i++) {
                s += pow(a.get(i) - b.get(i), 2);
            }
            return sqrt(s);
        }

        class Event {

            private:

                Entity initialState;
                Entity finalState;
                Entity action;

            public:

                Event(Entity initialState, Entity finalState, Entity action): initialState(initialState), 
                                                                              finalState(finalState), 
                                                                              action(action) {}

                Event(initializer_list<Entity> initList) {
                    int i = 0;
                    for (const auto& val : initList) {
                        if (i == 0) {
                            initialState = val;
                        } else if (i == 1) {
                            finalState = val;
                        } else if (i == 2) {
                            action = val;
                        } else {
                            break;
                        }
                        i++;
                    }
                    if (i < 2) {
                        notif::fatal("NyxNG: not enough Entities in initializer list to create Event");
                    }
                }

                const inline Entity getInitialState() { return initialState; }
                const inline Entity getFinalState() { return finalState; }
                const inline Entity getAction() { return action; }

        };

        class Memory: public sll::SecureLinkedList<Event> {

            public:

                Memory() {
                    head = nullptr;
                    locked = false;
                }

                ~Memory() {
                    sll::SecureLinkedListElement<Event>* current = head;
                    sll::SecureLinkedListElement<Event>* nextNode;

                    while (current != nullptr) {
                        nextNode = current->getNext();
                        delete current;
                        current = nextNode;
                    }
                }

        };

    };

    namespace modules {

        class Module {

            public:


                virtual string name() = 0;

                string repr() { 
                    stringstream ss;
                    ss << "<NyxNG-1 module: " << name() << ">";
                    return ss.str();
                }

        };

        class InterfaceModule: public Module {

            private:

                internal::Entity state;

            public:

                string name() override { return (string)("Interface"); }

                virtual internal::Entity delta(internal::Entity initial, internal::Entity action) = 0;

                internal::Entity getState() { return state; }
                void setState(internal::Entity s) { state = s; }
                internal::Event takeAction(MemoryModule* mm, internal::Entity action) { 
                    initialState = state;
                    setState(delta(state, action)); 
                    finalState = state;
                    internal::Event evt(initialState, finalState, action);
                    mm->record(evt);
                    return evt;
                }

        };

        class DoctrineModule final: public Module {

            public:

                string name() override { return (string)("Doctrine"); }

                // Doctrine module!

                class Doctrine {

                    public:

                        virtual double weight(internal::Entity initial, internal::Entity action, internal::Entity desiredFinal);

                };

                sll::SecureLinkedList<Doctrine> doctrines;

                void addDoctrine(Doctrine doct) { doctrines.append(doct); }
                void forgetDoctrine(int index) { doctrines.remove(index); }
                Doctrine get(int index) { return doctrines.get(index); }
                inline double getWeight(int index, internal::Entity initial, internal::Entity action, internal::Entity desiredFinal) {
                    return get(index).weight(initial, action, desiredFinal);
                }
                double weight(internal::Entity initial, internal::Entity action, internal::Entity desiredFinal) {
                    double result = 0;
                    for (int i = 0; i < doctrines.getLength(); i++) {
                        result += getWeight(i, initial, action, desiredFinal);
                    }
                    return result;
                }

        };

        class DirectorModule final: public Module {

            private:

                internal::Entity directive;

            public:

                string name() override { return (string)("Director"); }

                internal::Entity getDirective() { return directive; }
                internal::Entity setDirective(internal::Entity dcv) { directive = dcv; }

        };

        class MemoryModule final: public Module {

            private:

                sll::SecureLinkedList<internal::Memory> memories;
                int currentMemoryIndex = -1;

            public:

                MemoryModule() { nextMemory(); }

                void nextMemory() {
                    currentMemoryIndex++;
                    memories.set(currentMemoryIndex, internal::Memory());
                }

                void record(internal::Event evt) {
                    memories.get(currentMemoryIndex).append(evt);
                }

                internal::Memory get(int index) { return memories.get(index); }

        };

    };

    namespace agent {

        class NyxNGAgent {

            private:

                modules::InterfaceModule* ifm;
                modules::DoctrineModule* dom;
                modules::DirectorModule* dim;
                modules::MemoryModule* mm;
                sda::SDA<internal::Entity> actions;
                bool engaged = false;

            public:

                NyxNGAgent(modules::InterfaceModule* ifm_, 
                        modules::DoctrineModule* dom_, 
                        modules::DirectorModule* dim_, 
                        modules::MemoryModule* mm_, 
                        sda::SDA<internal::Entity> actions_) {
                    ifm = ifm_;
                    dom = dom_;
                    dim = dim_;
                    mm = mm_;
                    actions = actions_;
                }

                void engage() { engaged = true; }
                void disengage() { engaged = false; }
                void reengage() { mm->nextMemory(); engaged = true;}
                void direct(internal::Entity dcv) { dim->setDirective(dcv); }
                void takeAction(internal::Entity action) { ifm->takeAction(mm, action); }

                void iterate() {

                    if (engaged) {

                    } else {
                        
                    }

                }

        };

    };

};