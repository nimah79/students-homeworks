
public class ConferenceArticle extends Article {

	private String confereneName;
	private int year;
	private String location;

	public ConferenceArticle(String title, int coverNumber, int startPage,
			int finishPage, String confereneName, int year, String location) {
		super(title, coverNumber, startPage, finishPage);
		this.confereneName = confereneName;
		this.year = year;
		this.location = location;
	}

	public double getScore() {
		double score = 1;
		int authorsCount = this.authors.size();
		if (authorsCount == 2) {
			score += 0.8;
		} else if (authorsCount >= 3) {
			score += 0.6;
		}
		return score;
	}

	public String getConferenceName() {
		return this.confereneName;
	}

	public int getYear() {
		return this.year;
	}

	public String getLocation() {
		return this.location;
	}

}
