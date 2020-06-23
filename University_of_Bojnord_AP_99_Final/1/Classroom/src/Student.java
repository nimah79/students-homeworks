import java.util.*;

public class Student {
	
	private String name;
	private Map<Lesson, ArrayList<Item>> items = new HashMap<>();
	
	public Student(String name) {
		this.name = name;
	}
	
	public void addItem(Lesson lesson, Item item) {
		lesson.addItem(item);
		if (!this.items.containsKey(lesson))
			this.items.put(lesson, new ArrayList<Item>());
		this.items.get(lesson).add(item);
	}
	
	public String getName() {
		return this.name;
	}
	
	public double getAverageScore() {
		int itemsCount = 0;
		double sum = 0;
		for (ArrayList<Item> itemsList : items.values()) {
			for (Item item : itemsList) {
				sum += item.getFinalScore();
				itemsCount++;
			}
		}
		if (itemsCount == 0)
			return 0;
		return sum / itemsCount;
	}

}
