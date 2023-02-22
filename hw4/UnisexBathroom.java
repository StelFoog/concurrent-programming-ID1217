package hw4;

import java.sql.Timestamp;
import java.text.SimpleDateFormat;
import java.util.Random;

public class UnisexBathroom {
	final static int MAX_VISITS = 100;
	final static int MAX_MEN = 10;
	final static int MAX_WOMEN = 10;

	final static int MIN_WORK_TIME = 2;
	final static int MAX_WORK_TIME = 7;
	final static int MIN_BATHROOM_TIME = 1;
	final static int MAX_BATHROOM_TIME = 3;

	final static boolean WOMAN = true;
	final static boolean MAN = false;

	public class Person implements Runnable {
		private Random rand = new Random();
		private int id, visits, visit;
		private boolean gender;

		public Person(int id, int visits, boolean gender) {
			this.id = id;
			this.visits = visits;
			this.gender = gender;
		}

		private int randRange(int min, int max) {
			return rand.nextInt(max - min + 1) + min;
		}

		public int getVisit() {
			return this.visit + 1;
		}

		public void run() {
			try {
				for (this.visit = 0; this.visit < visits; this.visit++) {
					// Work time
					Thread.sleep(randRange(MIN_WORK_TIME, MAX_WORK_TIME) * 1000);

					// Enter bathroom
					if (this.gender == MAN) {
						UnisexBathroom.this.manEnter(this);
					} else {
						UnisexBathroom.this.womanEnter(this);
					}

					// Bathroom time
					int bathroomTime = randRange(MIN_BATHROOM_TIME, MAX_BATHROOM_TIME);
					Thread.sleep(bathroomTime * 1000);

					// Exit bathroom
					if (this.gender == MAN) {
						System.out.println(timestamp() + " 🚹 #" + this.id + " left after " + bathroomTime + "s");
						UnisexBathroom.this.manExit(this);
					} else {
						System.out.println(timestamp() + " 🚺 #" + this.id + " left after " + bathroomTime + "s");
						UnisexBathroom.this.womanExit(this);
					}
				}
			} catch (InterruptedException e) {
				Thread.currentThread().interrupt();
			}
		}
	}

	public enum BathroomState {
		MenEntering,
		MenLeaving,
		WomenEntering,
		WomenLeaving
	}

	private int menWaiting = 0, womenWaiting = 0;
	private int menInBathroom = 0, womenInBathroom = 0;
	private BathroomState bathroomState = null;

	public UnisexBathroom() {
	}

	public synchronized void manEnter(Person man) throws InterruptedException {
		menWaiting++;
		while (womenInBathroom > 0 || bathroomState == BathroomState.WomenEntering
				|| (bathroomState == BathroomState.MenLeaving && womenWaiting > 0))
			wait();

		menWaiting--;
		if (bathroomState != BathroomState.MenLeaving)
			bathroomState = BathroomState.MenEntering;
		System.out.println(timestamp() + " 🚹 #" + man.id + " enters bathroom - Visit " + man.getVisit());
		menInBathroom++;
	}

	public synchronized void manExit(Person man) {
		bathroomState = BathroomState.MenLeaving;
		menInBathroom--;

		if (menInBathroom == 0) {
			if (womenWaiting > 0)
				bathroomState = BathroomState.WomenEntering;
			else
				bathroomState = null;
		}

		notifyAll();
	}

	public synchronized void womanEnter(Person woman) throws InterruptedException {
		womenWaiting++;
		while (menInBathroom > 0 || bathroomState == BathroomState.MenEntering
				|| (bathroomState == BathroomState.WomenLeaving && menWaiting > 0))
			wait();

		womenWaiting--;
		if (bathroomState != BathroomState.WomenLeaving)
			bathroomState = BathroomState.WomenEntering;
		System.out.println(timestamp() + " 🚺 #" + woman.id + " enters bathroom - Visit " + woman.getVisit());
		womenInBathroom++;
	}

	public synchronized void womanExit(Person woman) {
		bathroomState = BathroomState.WomenLeaving;
		womenInBathroom--;

		if (womenInBathroom == 0) {
			if (menWaiting > 0)
				bathroomState = BathroomState.MenEntering;
			else
				bathroomState = null;
		}

		notifyAll();
	}

	public static int parseArg(String[] args, int index, int maximum) {
		if (args.length < index + 1)
			return maximum;
		int value = Integer.parseInt(args[index]);
		if (value > maximum || value < 1)
			return maximum;
		return value;
	}

	private static final SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss.SSS");

	private static String timestamp() {
		return sdf.format(new Timestamp(System.currentTimeMillis()));
	}

	public static void main(String[] args) {
		int numVisits = parseArg(args, 0, MAX_VISITS);
		int numMen = parseArg(args, 1, MAX_MEN);
		int numWomen = parseArg(args, 2, MAX_WOMEN);

		UnisexBathroom bathroom = new UnisexBathroom();

		for (int i = 0; i < numMen; i++) {
			UnisexBathroom.Person man = bathroom.new Person(i, numVisits, MAN);
			new Thread(man).start();
		}
		for (int i = 0; i < numWomen; i++) {
			UnisexBathroom.Person woman = bathroom.new Person(i, numVisits, WOMAN);
			new Thread(woman).start();
		}

		System.out.println("All men and women created\n");
	}
}
