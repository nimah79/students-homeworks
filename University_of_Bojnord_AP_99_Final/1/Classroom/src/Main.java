
public class Main {

	public static void main(String[] args) {
		Student student = new Student("Ali Alavi");
		Lesson lesson1 = new Lesson("AP", 3);
		student.addItem(lesson1, new Quiz("Quiz 1", 100));
		student.addItem(lesson1, new Homework("HW 1", 100, 10, 11));
		System.out.println(student.getAverageScore());
		System.out.println(lesson1.getAverageScore());
		Lesson lesson2 = new Lesson("DS", 3);
		student.addItem(lesson2, new Quiz("1", 100));
		student.addItem(lesson2, new Homework("1", 100, 4, 4));
		System.out.println(student.getAverageScore());
		System.out.println(lesson2.getAverageScore());
	}

}
