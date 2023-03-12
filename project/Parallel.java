package project;

import java.util.Random;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class Parallel {
	public static class Body {
		double posX;
		double posY;
		double velX;
		double velY;
		double[] forceX;
		double[] forceY;
		double mass;

		public Body(double posX, double posY, double mass, int numWorkers) {
			this.posX = posX;
			this.posY = posY;
			this.velX = 0;
			this.velY = 0;
			this.forceX = new double[numWorkers];
			this.forceY = new double[numWorkers];
			this.mass = mass;
		}

		public double dist(Body that) {
			return Math.sqrt(Math.pow(this.posX - that.posX, 2) + Math.pow(this.posY - that.posY, 2));
		}
	}

	public class Worker extends Thread {
		int id;
		CyclicBarrier barrier;

		public Worker(int id, CyclicBarrier barrier) {
			this.id = id;
			this.barrier = barrier;
		}

		public void await() {
			try {
				barrier.await();
			} catch (InterruptedException e) {
			} catch (BrokenBarrierException e) {
				System.err.println("Broken!");
			}

		}

		@Override
		public void run() {
			for (int i = 0; i < numSteps; i++) {
				// System.out.println("Run nr: " + i);
				Parallel.this.calculateForces(id);
				// System.out.println("Calc nr: " + i);
				await();
				// System.out.println("Wait nr: " + i);
				Parallel.this.moveBodies(id);
				// System.out.println("Move nr: " + i);
				await();
			}
		}
	}

	static final double G = 6.67e-11;
	static final double DT = 1;
	static final int MAX_BODIES = 240;
	static final int MAX_STEPS = 500000;
	static final int MAX_WORKERS = 4;
	static final int BODY_MASS = 2;
	static int gnumBodies;
	static int numSteps;
	static int numWorkers;
	static int printBodies;
	static int printNumBodies;
	Body[] bodies;

	public Parallel() {
		bodies = new Body[gnumBodies];
		Random rand = new Random();
		for (int i = 0; i < gnumBodies; i++) {
			bodies[i] = new Body(rand.nextDouble() * 10 - 5, rand.nextDouble() * 10 - 5, BODY_MASS, numWorkers);
		}
	}

	public void calculateForces(int w) {
		for (int i = w; i < gnumBodies; i += numWorkers) {
			for (int j = i + 1; j < gnumBodies; j++) {
				Body a = bodies[i], b = bodies[j];
				double distance = a.dist(b);
				double magnitude = (G * a.mass * b.mass) / (distance * distance);
				double dirX = b.posX - a.posX;
				double dirY = b.posY - a.posY;
				a.forceX[w] += magnitude * dirX / distance;
				b.forceX[w] -= magnitude * dirX / distance;
				a.forceY[w] += magnitude * dirY / distance;
				b.forceY[w] -= magnitude * dirY / distance;
			}
		}
	}

	public void moveBodies(int w) {
		for (int i = w; i < gnumBodies; i += numWorkers) {
			Body bod = bodies[i];
			double forceX = 0, forceY = 0;
			for (int k = 0; k < numWorkers; k++) {
				forceX += bod.forceX[k];
				bod.forceX[k] = 0;
				forceY += bod.forceY[k];
				bod.forceY[k] = 0;
			}

			double dVelX = forceX / bod.mass * DT;
			double dVelY = forceY / bod.mass * DT;
			double dPosX = (bod.velX + dVelX / 2) * DT;
			double dPosY = (bod.velY + dVelY / 2) * DT;

			bod.velX += dVelX;
			bod.velY += dVelY;
			bod.posX += dPosX;
			bod.posY += dPosY;
		}
	}

	public void printBodies(String title) {
		if (printBodies == 0)
			return;
		System.out.println("\n" + title);
		for (int i = 0; i < printNumBodies; i++)
			System.out.format("Body #%d\n  x = %f\n  y = %f\n", i + 1, bodies[i].posX, bodies[i].posY);
	}

	public static int parseArg(String[] args, int index, int max, int fallback, int min) {
		if (args.length < index + 1)
			return fallback;
		int val = Integer.parseInt(args[index]);
		return (val > max || val < min) ? max : val;
	}

	public static void main(String[] args) {
		gnumBodies = parseArg(args, 0, MAX_BODIES, MAX_BODIES, 1);
		numSteps = parseArg(args, 1, MAX_STEPS, MAX_STEPS, 1);
		numWorkers = parseArg(args, 2, MAX_WORKERS, MAX_WORKERS, 1);
		printBodies = parseArg(args, 3, 1, 1, 0);
		printNumBodies = parseArg(args, 4, gnumBodies, 5, 1);

		System.out.println("gnumBodies = " + gnumBodies);
		System.out.println("numSteps = " + numSteps);
		System.out.println("numWorkers = " + numWorkers);
		System.out.println("printBodies = " + printBodies);
		System.out.println("printNumBodies = " + printNumBodies);

		Parallel nBodies = new Parallel();
		Worker[] workers = new Worker[numWorkers];
		CyclicBarrier barrier = new CyclicBarrier(numWorkers);
		nBodies.printBodies("Starting body positions:");

		long start = System.nanoTime();
		for (int i = 0; i < numWorkers; i++) {
			workers[i] = nBodies.new Worker(i, barrier);
			workers[i].start();
		}

		for (int i = 0; i < numWorkers; i++) {
			try {
				workers[i].join();
			} catch (InterruptedException e) {
			}
		}
		long time = System.nanoTime() - start;

		nBodies.printBodies("Final body positions:");
		System.out.println("\nDone in " + time * Math.pow(10, -9) + "s");
	}
}
