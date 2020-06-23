import java.util.ArrayList;

public class Main {

	public static void main(String[] args) {
		FacultyMember fm1 = new FacultyMember(1, "Naghi Naghavi", 1, "Computer");
		FacultyMember fm2 = new FacultyMember(2, "Taghi Taghavi", 3, "Computer");
		Person p1 = new Person(3, "A");
		Person p2 = new Person(4, "B");
		ArrayList<Article> articles = new ArrayList<>();
		Article a1 = new JournalArticle("Journal Article 1", 1, 5, 10, "Some Journal Name", p1);
		a1.getAuthors().add(fm1);
		a1.getAuthors().add(p1);
		articles.add(a1);
		Article a2 = new JournalArticle("Journal Article 2", 2, 30, 26, "Some Journal Name", p2);
		a2.getAuthors().add(fm1);
		a2.getAuthors().add(p1);
		a2.getAuthors().add(p2);
		articles.add(a2);
		Article a3 = new ConferenceArticle("Conference Article 1", 3, 1, 50, "Some Conference Name", 2020, "Tehran");
		a3.getAuthors().add(fm1);
		a3.getAuthors().add(p1);
		articles.add(a3);
		Article a4 = new ConferenceArticle("Conference Article 2", 4, 1, 60, "Some Conference Name", 2019, "London");
		a4.getAuthors().add(fm1);
		a4.getAuthors().add(fm2);
		a4.getAuthors().add(p2);
		articles.add(a4);
		System.out.println(fm1.getTotalScore(articles));
		System.out.println(fm2.getTotalScore(articles));
	}

}
