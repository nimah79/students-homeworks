import java.util.ArrayList;

public class Lesson {

	private String name;
	private int units;
	private ArrayList<Item> items = new ArrayList<>();

	public Lesson(String name, int units) {
		this.name = name;
		this.units = units;
	}

	public void addItem(Item item) {
		this.items.add(item);
	}

	public String getName() {
		return this.name;
	}

	public int getUnits() {
		return this.units;
	}

	public double getAverageScore() {
		int itemsCount = 0;
		double sum = 0;
		for (Item item : this.items) {
			sum += item.getFinalScore();
			itemsCount++;
		}
		if (itemsCount == 0)
			return 0;
		return sum / itemsCount;
	}

}
