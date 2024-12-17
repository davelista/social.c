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

new entities or relationships are instantiated (**addent** and **addrel**) and removed (**delent** and **delrel**). The **report** command verifies the correct handling of the given case.  

The **report** outputs, in alphabetical order, all the relationships managed by the system, providing for each relationship the entity receiving the highest number of that relationship. If multiple entities receive the same number of a specific relationship, all are reported in alphabetical order before displaying the count of occurrences.

## Implementation  
The data structures used to manage the system are of three types:  
- **Hash Table**  
- **Doubly Linked List**  
- **Red-Black Tree**  

As soon as an **entity** starts being monitored, a node is inserted into a hash table whose index is determined using the [djb2 hash function](http://www.cse.yorku.ca/~oz/hash.html) that operates on the entity's name.  

When a **relationship** starts being monitored, it is inserted into a **doubly linked list** so that subsequent insertions of the same relationship only update the contents of the structures within each node.  

Each node in the relationship list contains:  
- Name  
- Maximum occurrences  
- Red-Black Tree of receiving entities  
- Red-Black Tree for the report  

This design ensures that for any operation involving an **entity** or **relationship**, the node related to the **entity** in question is added (or removed) in the receiving entities' tree, and, if needed, the counter and the related report tree are updated. The nodes of the receiving entities' tree each contain another tree that tracks the entities from which the relationship is received.

The main **algorithms** used for managing the described data structures focus on adding and removing nodes in a **red-black tree** and traversing it through [tree traversal visits](https://en.wikipedia.org/wiki/Tree_traversal).
