
public class JournalArticle extends Article {

	private String journalName;
	private Person editor;

	public JournalArticle(String title, int coverNumber, int startPage,
			int finishPage, String journalName, Person editor) {
		super(title, coverNumber, startPage, finishPage);
		this.journalName = journalName;
		this.editor = editor;
	}

	public double getScore() {
		double score = 3;
		int authorsCount = this.authors.size();
		if (authorsCount == 2) {
			score += 0.85;
		} else if (authorsCount >= 3) {
			score += 0.75;
		}
		return score;
	}

	public String getJournalName() {
		return this.journalName;
	}

	public Person getEditor() {
		return this.editor;
	}

}
