
public class Homework extends Item {

	private String name;
	private double score;
	private int maxDate;
	private int deliveryDate;

	public Homework(String name, double score, int maxDate, int deliveryDate) {
		this.name = name;
		this.score = score;
		this.maxDate = maxDate;
		this.deliveryDate = deliveryDate;
	}

	public double getFinalScore() {
		if (deliveryDate <= maxDate)
			return score;
		return score * (1 - 0.05 * (deliveryDate - maxDate));
	}

	public String getName() {
		return this.name;
	}

	public double getScore() {
		return this.score;
	}

	public double getMaxDate() {
		return this.maxDate;
	}

	public double getDeliveryDate() {
		return this.deliveryDate;
	}

}
