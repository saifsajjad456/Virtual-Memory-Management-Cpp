#include <iostream>

#define PAGE_SIZE 256
#define NUM_PAGES 4
#define MAX_REFERENCES 100

using namespace std;

class VirtualMemory {
public:
    int memory[NUM_PAGES];
    int page_table[NUM_PAGES];
    int page_count;
    int recent_use[NUM_PAGES];
    int time_counter;
    int future_references[MAX_REFERENCES];
    int ref_length;
    int algo_choice;

    VirtualMemory(int algo, int refs[], int length) {
        algo_choice = algo;
        for (int i = 0; i < length; i++) {
            future_references[i] = refs[i];
        }
        ref_length = length;
        init_memory();
    }

    void init_memory() {
        for (int i = 0; i < NUM_PAGES; i++) {
            memory[i] = -1;
            page_table[i] = -1;
            recent_use[i] = -1;
        }
        page_count = 0;
        time_counter = 0;
    }

    int translate_address(int virtual_address) {
        int page_number = virtual_address / PAGE_SIZE;
        int offset = virtual_address % PAGE_SIZE;

        if (page_number < NUM_PAGES && page_table[page_number] != -1) {
            if (algo_choice == 3) {
                recent_use[page_table[page_number]] = time_counter++;
            }
            return page_table[page_number] * PAGE_SIZE + offset;
        } else {
            return -1;
        }
    }

    void add_page_fifo(int page_number) {
        if (page_count < NUM_PAGES) {
            for (int i = 0; i < NUM_PAGES; i++) {
                if (memory[i] == -1) {
                    memory[i] = page_number;
                    page_table[page_number] = i;
                    recent_use[i] = time_counter++;
                    page_count++;
                    break;
                }
            }
        } else {
            int replaced_page = memory[0];
            for (int i = 0; i < NUM_PAGES - 1; i++) {
                memory[i] = memory[i + 1];
                page_table[memory[i]] = i;
                recent_use[i] = recent_use[i + 1];
            }
            memory[NUM_PAGES - 1] = page_number;
            page_table[page_number] = NUM_PAGES - 1;
            recent_use[NUM_PAGES - 1] = time_counter++;
            page_table[replaced_page] = -1;
        }
    }

    void add_page_lru(int page_number) {
        if (page_count < NUM_PAGES) {
            for (int i = 0; i < NUM_PAGES; i++) {
                if (memory[i] == -1) {
                    memory[i] = page_number;
                    page_table[page_number] = i;
                    recent_use[i] = time_counter++;
                    page_count++;
                    return;
                }
            }
        } else {
            int lru_index = 0;
            for (int i = 1; i < NUM_PAGES; i++) {
                if (recent_use[i] < recent_use[lru_index]) {
                    lru_index = i;
                }
            }
            int replaced_page = memory[lru_index];
            memory[lru_index] = page_number;
            page_table[replaced_page] = -1;
            page_table[page_number] = lru_index;
            recent_use[lru_index] = time_counter++;
        }
    }

    void add_page_optimal(int page_number, int current_index) {
        if (page_count < NUM_PAGES) {
            for (int i = 0; i < NUM_PAGES; i++) {
                if (memory[i] == -1) {
                    memory[i] = page_number;
                    page_table[page_number] = i;
                    page_count++;
                    return;
                }
            }
        } else {
            int farthest = -1;
            int index_to_replace = -1;

            for (int i = 0; i < NUM_PAGES; i++) {
                int page = memory[i];
                int j;
                for (j = current_index + 1; j < ref_length; j++) {
                    if (future_references[j] == page)
                        break;
                }
                if (j > farthest) {
                    farthest = j;
                    index_to_replace = i;
                }
            }

            int replaced_page = memory[index_to_replace];
            memory[index_to_replace] = page_number;
            page_table[replaced_page] = -1;
            page_table[page_number] = index_to_replace;
        }
    }

    void add_page(int page_number, int current_index) {
        if (algo_choice == 1) {
            add_page_fifo(page_number);
        } else if (algo_choice == 2) {
            add_page_optimal(page_number, current_index);
        } else if (algo_choice == 3) {
            add_page_lru(page_number);
        }
    }

    void display_memory() {
        cout << "Current Memory State: ";
        for (int i = 0; i < NUM_PAGES; i++) {
            cout << memory[i] << " ";
        }
        cout << endl;
    }
};

int main() {
    while (true) {
        int algo;
        cout << "\nChoose Page Replacement Algorithm:\n";
        cout << "1. FIFO\n2. OPTIMAL\n3. LRU\n4. Exit\nEnter choice (1-4): ";
        cin >> algo;

        if (algo == 4) {
            cout << "Exiting the program.\n";
            break;
        }

        if (algo < 1 || algo > 4) {
            cout << "Invalid choice. Please enter a number between 1 and 4.\n";
            continue;
        }

        // Sample reference string
        int refs[MAX_REFERENCES] = {0, 1, 2, 3, 4, 0, 2, 5, 8, 0};
        int ref_len = 10;

        VirtualMemory vm(algo, refs, ref_len);

        for (int i = 0; i < ref_len; i++) {
            cout << "Adding page " << refs[i] << endl;
            vm.add_page(refs[i], i);
            vm.display_memory();
        }

        for (int i = 0; i < ref_len; i++) {
            int address = refs[i] * PAGE_SIZE + 10;
            int physical_address = vm.translate_address(address);
            if (physical_address != -1) {
                cout << "Virtual address " << address << " translates to physical address " << physical_address << endl;
            } else {
                cout << "Page fault for virtual address " << address << endl;
            }
        }

        cout << "\n--- Simulation Complete ---\n";
    }
    return 0;
}

