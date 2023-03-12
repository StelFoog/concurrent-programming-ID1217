package project;

import java.util.Random;

public class SequentialBH {
	public static class Quad {
		double midX;
		double midY;
		double len;

		public Quad(double midX, double midY, double len) {
			this.midX = midX;
			this.midY = midY;
			this.len = len;
		}

		public boolean contains(double x, double y) {
			double halfLength = len / 2.0;
			if (midX - halfLength > x || midX + halfLength < x)
				return false;
			if (midY - halfLength > y || midY + halfLength < y)
				return false;
			return true;
		}

		public Quad tL() {
			double quartLength = len / 4.0;
			return new Quad(midX - quartLength, midY + quartLength, quartLength * 2);
		}

		public Quad tR() {
			double quartLength = len / 4.0;
			return new Quad(midX + quartLength, midY + quartLength, quartLength * 2);
		}

		public Quad bL() {
			double quartLength = len / 4.0;
			return new Quad(midX - quartLength, midY - quartLength, quartLength * 2);
		}

		public Quad bR() {
			double quartLength = len / 4.0;
			return new Quad(midX + quartLength, midY - quartLength, quartLength * 2);
		}
	}

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

		public boolean isIn(Quad quad) {
			return quad.contains(posX, posY);
		}

		public Body add(Body that) {
			double m = this.mass + that.mass;
			double posX = (this.posX * this.mass + that.posX * that.mass) / m;
			double posY = (this.posY * this.mass + that.posY * that.mass) / m;
			return new Body(posX, posY, m);
		}

		public void addForce(Body that) {
			double distance = this.dist(that);
			double dirX = that.posX - this.posX;
			double dirY = that.posY - this.posY;

			double magnitude = (G * this.mass * that.mass) / (distance * distance);
			this.forceX += magnitude * dirX / distance;
			this.forceY += magnitude * dirY / distance;
		}

		public void move() {
			double dVelX = this.forceX / this.mass * DT;
			double dVelY = this.forceY / this.mass * DT;
			double dPosX = (this.velX + dVelX / 2) * DT;
			double dPosY = (this.velY + dVelY / 2) * DT;

			this.velX += dVelX;
			this.velY += dVelY;
			this.posX += dPosX;
			this.posY += dPosY;
		}
	}

	public static class BHTree {
		Body body;
		Quad quad;
		BHTree tL;
		BHTree tR;
		BHTree bL;
		BHTree bR;

		public BHTree(Quad quad) {
			this.body = null;
			this.quad = quad;
			this.tL = null;
			this.tR = null;
			this.bL = null;
			this.bR = null;
		}

		public void insert(Body b) {
			if (this.body == null) {
				this.body = b;
				return;
			}

			if (!this.isExternal()) {
				this.body = this.body.add(b);
				this.addToChild(b);
				return;
			}

			this.tL = new BHTree(this.quad.tL());
			this.tR = new BHTree(this.quad.tR());
			this.bL = new BHTree(this.quad.bL());
			this.bR = new BHTree(this.quad.bR());

			this.addToChild(this.body);
			this.addToChild(b);

			this.body = this.body.add(b);
		}

		private void addToChild(Body b) {
			if (b.isIn(this.tL.quad)) {
				this.tL.insert(b);
			} else if (b.isIn(this.tR.quad)) {
				this.tR.insert(b);
			} else if (b.isIn(this.bL.quad)) {
				this.bL.insert(b);
			} else if (b.isIn(this.bR.quad)) {
				this.bR.insert(b);
			}
		}

		public void updateForce(Body b) {
			if (this.body == null || this.body.equals(b))
				return;

			if (this.isExternal() || (this.quad.len / this.body.dist(b)) < far) {
				b.addForce(this.body);
				return;
			}

			this.tL.updateForce(b);
			this.tR.updateForce(b);
			this.bL.updateForce(b);
			this.bR.updateForce(b);
		}

		// Has no children, is internal otherwise
		public boolean isExternal() {
			return tL == null && tR == null && bL == null && bR == null;
		}
	}

	static final double G = 6.67e-11;
	static final double DT = 1;
	static final int MAX_BODIES = 240;
	static final int MAX_STEPS = 500000;
	static final double DEFAULT_FAR = 5;
	static final int BODY_MASS = 2;
	static int gnumBodies;
	static int numSteps;
	static double far;
	static int printBodies;
	static int printNumBodies;
	static Body[] bodies;

	public static double findRootLen() {
		double max = 0;
		for (int i = 0; i < gnumBodies; i++) {
			double localMax = Math.max(Math.abs(bodies[i].posX), Math.abs(bodies[i].posY));
			if (localMax > max)
				max = localMax;
		}
		return (max * 1.001) * 2;
	}

	public static void printBodies(String title) {
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
		far = args.length < 3 ? DEFAULT_FAR : Double.parseDouble(args[2]);
		printBodies = parseArg(args, 3, 1, 0, 0);
		printNumBodies = parseArg(args, 4, gnumBodies, 5, 1);

		System.out.println("gnumBodies = " + gnumBodies);
		System.out.println("numSteps = " + numSteps);
		System.out.println("far = " + far);
		System.out.println("printBodies = " + printBodies);
		System.out.println("printNumBodies = " + printNumBodies);

		Random rand = new Random();
		bodies = new Body[gnumBodies];
		for (int i = 0; i < gnumBodies; i++)
			bodies[i] = new Body((rand.nextDouble() * 10) - 5, (rand.nextDouble() * 10) - 5, BODY_MASS);
		printBodies("Starting body positions:");

		long start = System.nanoTime();
		for (int step = 0; step < numSteps; step++) {
			Quad q = new Quad(0, 0, findRootLen());

			BHTree tree = new BHTree(q);
			for (int i = 0; i < gnumBodies; i++)
				tree.insert(bodies[i]);

			for (int i = 0; i < gnumBodies; i++) {
				bodies[i].forceX = bodies[i].forceY = 0;
				tree.updateForce(bodies[i]);
			}

			for (int i = 0; i < gnumBodies; i++)
				bodies[i].move();
		}
		long time = System.nanoTime() - start;

		printBodies("Final body positions:");
		System.out.println("\nDone in " + time * Math.pow(10, -9) + "s");
	}
}
