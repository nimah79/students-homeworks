import java.util.ArrayList;

public class FacultyMember extends Person {

	private int title;
	private String facultyName;

	public FacultyMember(int id, String name, int title, String facultyName) {
		super(id, name);
		this.title = title;
		this.facultyName = facultyName;
	}

	public int getTitle() {
		return this.title;
	}

	public String getFacultyName() {
		return this.facultyName;
	}

	public double getTotalScore(ArrayList<Article> articles) {
		double totalScore = 0;
		for (Article article : articles) {
			if (article.getAuthors().contains(this)) {
				totalScore += article.getScore();
			}
		}
		return totalScore;
	}

}
