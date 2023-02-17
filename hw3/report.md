# Report for Homework #3

## `unisexBathroom.c`

Creates a set number of men and women who all have to go to the bathroom a set amount of time.

Each person, for each visit, follows this pattern:

1. Work for a random amount of time (`sleep`)
2. Attempt to enter the bathroom by taking a critical lock and checking if there are people of other gender in the bathroom or people of the same geneder have already begun leaving the bathroom.
   1. If they can't enter; increase the number of own gender standing in line, release the critical lock and wait until it's announced that they can enter.
   2. If allowed to enter procede without other action
3. When entering the bathroom check if there are people of the same gender waiting to come
   1. If there are, announce to them that they can enter
   2. Otherwise release the critical lock (which they have either from not having to wait or through the announcement)
4. Enter the bathroom and spend a random amount of time there (`sleep`)
5. Take the critical lock and leave the bathroom
6. Check if they are the last to leave and people from the other gender are waiting to come in
   1. When true: Let a person from the other gender in (let them deal with the critical lock)
   2. Otherwise: Release the critical lock
7. Back to work

### Fairness

To ensure that one gender doesn't hog the bathroom for the entire day no one is allowed to enter the bathroom while people from their gender are in the process of leaving and people from the other gender are waiting to enter. The last person of a gender leaving also gives the critical lock to a person from the other gender if they have been waiting, which allowes them to keep it until everyone who were waiting has come in.
