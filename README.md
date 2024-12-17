# Final Exam of Algorithms and Data Structures - a.y. 2018-2019  
The goal of the project is to implement a system in standard C language capable of managing **entities** and **relationships**.

The instance to be managed is provided in a text file received as standard input, which defines the **entities** to be monitored and the **relationships** between them.

Using the following commands:

- **addent**  
- **addrel**  
- **delent**  
- **delrel**  
- **report**  
- **end**

new entities or relationships are instantiated (**addent** and **addrel**) and removed (**delent** and **delrel**). The **report** command outputs, in alphabetical order, all the relationships managed by the system. For each relationship, it provides the entity receiving the highest number of that relationship. If multiple entities receive the same number, all are displayed in alphabetical order before displaying the count of occurrences.

---

## **Data Structures**  

The program uses the following data structures:  

1. **Hash Table**  
   - **EntitiesHashTable (EHT)**: Stores entities and their associated relationships. Collisions are managed through chaining with additional entries.  
   - **RelationsHashTable (RHT)**: Stores relationships, uses a custom hash function, and handles collisions by redirecting entries.  

2. **Red-Black Tree (RB-Tree)**  
   - Manages **alphabetical ordering** of relationships.  
   - Keeps track of **destination entities** and the number of relationships for efficient searching and updating.
   
---

## **Hash Functions**  

- **djb2 Hash Function** (for strings): Used to calculate keys for both EHT and RHT.  
- **Custom Modulo Function** for entity names ensures efficient insertion and lookup.  

---

## **Main Algorithms**  

The implementation focuses on the following operations:  

1. **RB-Tree Operations**  
   - **Insertion** and **Deletion** of nodes in Red-Black Trees for ordered relationships and entity management.  
   - **Tree Traversal** for `report` output in **alphabetical order**.  
   - **Fix-Up Algorithms** to maintain Red-Black properties after updates.

2. **Hash Table Management**  
   - Insertion and collision resolution in both EHT and RHT.  
   - Efficient **search** operations for entities and relationships.  

3. **Command Parsing**  
   The `parse_command_line` function parses and processes user commands:  
   - **addent**: Adds a new entity.  
   - **delent**: Deletes an entity and all associated relationships.  
   - **addrel**: Adds a relationship between two entities.  
   - **delrel**: Removes a specific relationship.  
   - **report**: Outputs relationships in alphabetical order with the most frequent entity for each.  
   - **end**: Terminates the program.

---

## **Compilation and Execution**  

To compile the program:  
```bash
gcc -Wmaybe-uninitialized -Wuninitialized -Wall -pedantic -Werror -g3 main.c -o main
```

To execute the program using an input file and verify the output:  
```bash
cat input.txt | ./main > output.txt
diff output.txt public_output.txt
```

---

## **Report Output**  

The `report` command outputs the following:  
- Each relationship in **alphabetical order**.  
- The entity/entities receiving the **highest number** of that relationship.  
- Count of occurrences.  

If no relationships are present, the program outputs:  
```
none
```

---

## **Example Input**  

```text
addent A  
addent B  
addrel A B likes  
report  
end
```

### Corresponding Output  
```text
likes B 1;
```

---

## **Test Cases**  

The tests used to validate the implementation are available in another user's GitHub repository:  

- **Public Tests**: [Link](https://github.com/<username>/Progetto-API-2019/tree/master/Public_Tests)  
- **Private Tests**: [Link](https://github.com/<username>/Progetto-API-2019/tree/master/Private_Tests)  

These tests were used to verify the program's correctness, memory usage, and execution time.

---

## **Code Structure Overview**  

- **Main Program**: `main()` initializes data structures and processes input commands.  
- **Entities Management**: `eht_insert`, `delent`.  
- **Relationships Management**: `rht_insert`, `addrel`, `delrel`.  
- **Red-Black Tree Functions**: Manage insertion, deletion, and traversal.  
- **Report Command**: Outputs results using Red-Black Tree traversal.  

---

## **Efficiency**  

The program ensures efficient handling of entities and relationships by:  
- **O(log n)** operations for insertion, deletion, and lookup in Red-Black Trees.  
- **O(1)** average-time complexity for hash table lookups.  

---

This version of the system is robust, efficient, and capable of handling a large number of entities and relationships while maintaining correctness and performance.  
