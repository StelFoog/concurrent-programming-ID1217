# Report for Homework #4

## `UnisexBathroom.java`

Creates a set number of men and women who all have to go to the bathroom a set amount of time.

Each person, for each visit, follows this pattern:

1. Work for a random amount of time `Thread.sleep()`
2. Try to enter the bathroom by entering the queue
3. If there are people of the other geneder in the bathroom, the other gender is supposed to enter, or their own gender has begun leaving and there are people from the other gender in the queue; start waiting.
4. Once their allowed to enter they leave the queue and spend a random amount of time there `Thread.sleep()`.
5. When done leave the bathroom and check if the bathroom is empty, if it is:
   1. If people from the other gender are wating in the queue mark them as entering
   2. Otherwise mark the bathroom as empty
6. Notify all those waiting in queue to check if they are allowed to enter now

### Fairness

To ensure that one gender doesn't hog the bathroom for the entire day no one is allowed to enter the bathroom while people from their gender are in the process of leaving and people from the other gender are waiting to enter. The last person to leave the bathroom checks if people from the other gender are waiting and makes sure that only people from that gender are allowed to enter.
