# Habits #
These are things that apply to just about any situation, language, or employer

  * Documentation
    * Your peers should be able to read your code and figure out what's going on. Poorly documented code makes things harder for everyone, and should be avoided at all costs, unless you're writing disposable code.
  * Keep it Simple, Stupid (KISS)
    * It's easy to get caught up in your own little world when programming. If you find yourself thinking there must be a simpler way to accomplish a task, there probably is. Step back an consider your options.
  * Don't Repeat Yourself (The DRY Principle)
    * It can be tricky to always adhere to this principle, but there is always a way. And when you realize the specifications for your program have changed, you will be thankful for it. If you find yourself repeating a block of code, consider how you might refactor it.
  * Avoid premature optimization
    * This is another tricky one. As programmers we are constantly looking for more efficient ways to accomplish a task, and while this is a good habit when applied responsibly, it is all too easy to get caught up in micro-optimizations that will frequently lead to less readable code. So don't do that.
  * Favor the narrowest scope
    * You always want to place variables in the most narrow scope possible: it will make your code more readable, maintainable, and less error-prone.
  * Source Code/Version Control System
    * When using Github or a similar system, always test your code thoroughly before merging it with the main branch. Experimental code is fine as long as it stays away from the main branch until it has been proven and documented.
  * Peer Reviews
    * It is always helpful to have a another programmer review your code as we can easily overlook a detail or get caught up in some dense logic while programming. Besides that, we all have vastly different experiences and can always learn from one another. Just be respectful when reviewing or observing.
    * Another style of development is pair programming. Here, one person does the coding while the other person will handle the bulk of the complex logic and help spot errors in syntax and logic. This method will usually result in overall higher quality code.


# Coding Conventions #
These conventions may not apply to some languages or circles of programmers. That's okay as long as we are consistent as a team. Understand that by definition, coding conventions are not inherently correct; they are merely a way to standardize the code we write as a team. I will try to only include conventions that can be applied to a broad spectrum of C-like languages.

  * Indentation
    * When changing scope in your code, indent using 4 spaces. Many text-editors and IDEs will give you the option to automatically convert tabs to a number of spaces.
    * This one is important because switching platforms or uploading code to a Source Code Control System can wreak havoc on your code. Just this summer while I was getting acquainted with Github, I used tabs to indent my documentation, and when I uploaded my code, Github had shifted all my comments around (usually only slightly, but it was still very annoying).
  * Bracket usage
    * Many languages provide the option to forego brackets if s statement have only a single line of code to execute, but always use brackets when the option presents itself, unless you have several if/else statements back to back (in which case you should consider a switch statement, if possible)
```
      if(x == 1)
          x = 2;

       //Prefer this
       if(x == 1) 
       {
           x = 2;
       }
```

  * Egyptian Brackets vs ...Non-Egyptian Brackets
    * I didn't coin this term but I wish I had
    * Prefer Non-Egyptian Brackets
    * I don't know who started this Egyptian bracket nonsense or why it's even remotely popular, but it must stop. Personal bias aside, I find the alternative easier to read and deduce the scope of a ...scope.
```
    if(x == 1){
        x = 2;
    }

    //Prefer this
    if(x == 1)
    {
        x = 2;
    }
```

_I will add to this document as needed, but these are just a few basic guidelines to get us started. If you think I should add something, just let me know. For stricter standards, which I may draw from in the future, you may refer to Google's C++ Style Guide here:
https://google-styleguide.googlecode.com/svn/trunk/cppguide.html_