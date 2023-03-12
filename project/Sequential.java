package project;

import java.util.Random;

public class Sequential {
	public static class Body {
		double posX;
		double posY;
		double velX;
		double velY;
		double forceX;
		double forceY;
		double mass;

		public Body(double posX, double posY, double mass) {
			this.posX = posX;
			this.posY = posY;
			this.velX = 0;
			this.velY = 0;
			this.forceX = 0;
			this.forceY = 0;
			this.mass = mass;
		}

		public double dist(Body that) {
			return Math.sqrt(Math.pow(this.posX - that.posX, 2) + Math.pow(this.posY - that.posY, 2));
		}
	}

	/* SEQUENTIAL */

	static final double G = 6.67e-11;
	static final double DT = 1;
	static final int MAX_BODIES = 240;
	static final int MAX_STEPS = 500000;
	static final int BODY_MASS = 2;
	static int gnumBodies;
	static int numSteps;
	static int printBodies;
	static int printNumBodies;
	Body[] bodies;

	public Sequential() {
		bodies = new Body[gnumBodies];
		Random rand = new Random();
		for (int i = 0; i < gnumBodies; i++) {
			bodies[i] = new Body(rand.nextDouble() * 10 - 5, rand.nextDouble() * 10 - 5, BODY_MASS);
		}
	}

	public void calculateForces() {
		for (int i = 0; i < gnumBodies; i++) {
			for (int j = i + 1; j < gnumBodies; j++) {
				Body a = bodies[i], b = bodies[j];
				double distance = a.dist(b);
				double magnitude = (G * a.mass * b.mass) / (distance * distance);
				double dirX = b.posX - a.posX;
				double dirY = b.posY - a.posY;
				a.forceX += magnitude * dirX / distance;
				b.forceX -= magnitude * dirX / distance;
				a.forceY += magnitude * dirY / distance;
				b.forceY -= magnitude * dirY / distance;
			}
		}
	}

	public void moveBodies() {
		for (int i = 0; i < gnumBodies; i++) {
			Body bod = bodies[i];
			double dVelX = bod.forceX / bod.mass * DT;
			double dVelY = bod.forceY / bod.mass * DT;
			double dPosX = (bod.velX + dVelX / 2) * DT;
			double dPosY = (bod.velY + dVelY / 2) * DT;

			bod.velX += dVelX;
			bod.velY += dVelY;
			bod.posX += dPosX;
			bod.posY += dPosY;
			bod.forceX = bod.forceY = 0; // Reset forces
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
		printBodies = parseArg(args, 2, 1, 0, 0);
		printNumBodies = parseArg(args, 3, gnumBodies, 5, 1);

		System.out.println("gnumBodies = " + gnumBodies);
		System.out.println("numSteps = " + numSteps);
		System.out.println("printBodies = " + printBodies);
		System.out.println("printNumBodies = " + printNumBodies);

		Sequential nBodies = new Sequential();
		nBodies.printBodies("Starting body positions:");

		long start = System.nanoTime();
		for (int i = 0; i < numSteps; i++) {
			nBodies.calculateForces();
			nBodies.moveBodies();
		}
		long time = System.nanoTime() - start;

		nBodies.printBodies("Final body positions:");
		System.out.println("Done in " + time * Math.pow(10, -9) + "s");
	}
}
