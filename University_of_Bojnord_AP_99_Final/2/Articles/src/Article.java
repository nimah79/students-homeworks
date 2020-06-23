import java.util.ArrayList;

public abstract class Article {
	
	protected String title;
	protected int coverNumber;
	protected int startPage;
	protected int finishPage;
	protected ArrayList<Person> authors = new ArrayList<>();
	
	public Article(String title, int coverNumber, int startPage,
			int finishPage) {
		this.title = title;
		this.coverNumber = coverNumber;
		this.startPage = startPage;
		this.finishPage = finishPage;
	}

	public abstract double getScore();

	public String getTitle() {
		return this.title;
	}

	public int getCoverNumber() {
		return this.coverNumber;
	}

	public int getStartPage() {
		return this.startPage;
	}

	public int getFinishPage() {
		return this.finishPage;
	}

	public ArrayList<Person> getAuthors() {
		return this.authors;
	}

}
