geo_age_proximity_1.0
==========
02/26/2015

Mengdi Zhang 
-----------
mdzhangmd@gmail.com
-------------------

A demo for two dimensions(geo, age) proximity  user search in command-line interface.
To use gap just run make and then run "./test" in command-line.

-------------------
The Process :

###  step1. Load name simple data to memory

### step2. Generate 10,000,000 users data and store in a  2 levels trie-Tree structure, whose depth is 2 and width of  every non-leaf node is 32. 
Every non-leaf node in the trie tree is a char of BASE32, and every leaf node contain a users list, whose geohash code is the exact path on this trie tree.

### step3.  Input the query user's age and location.

### step4. Search the top10 similar users for the input query.
In this core step, the program firstly locate the geo hash index on the trie tree. Secondly, return the users list store in the trie tree leaf node, which are geo neihgbors of the input query. Then visit every user in this list and calculate the similar score between them and the input query . Finally, use heap sort method to sort the scores and return the top10 similar users;

##Note:

Total number of the faking users: 10,000,000.		
Generation time: around 10s.		
Search time: within 0.1s.		
Required memory: at least  1G.		

my experiment environment: Ubuntu 14.04 LTS, Memory 8G, Disk 10G, OS type 64-bit

There are still lots of work could be improved in data structure , such using hashmap instead vector for the trie tree , which could reduce the data generation time and increase the flexibility of more levels of the geo index trie tree.
