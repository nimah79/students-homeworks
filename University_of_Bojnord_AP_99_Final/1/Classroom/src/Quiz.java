
public class Quiz extends Item {

	private String name;
	private double score;

	public Quiz(String name, double score) {
		this.name = name;
		this.score = score;
	}

	public double getFinalScore() {
		return score;
	}

	public String getName() {
		return this.name;
	}

	public double getScore() {
		return this.score;
	}

}
