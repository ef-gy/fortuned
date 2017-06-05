# Fortune As A Service

[![Build Status](https://travis-ci.org/ef-gy/fortuned.svg?branch=master)](https://travis-ci.org/ef-gy/fortuned)

`fortuned` implements an HTTP API for retrieving fortune cookies, as normally
shown by the `fortune` programme on your computer. This allows access to these
cookies via your favorite web services. Because.

Yadda yadda leveraging yadda industry-grade yadda yadda RESTful yadda
disruptive yadda yadda revenue yadda killer app yadda convergence yadda robust
metrics yadda yadda game-changing. This paragraph was placed here in case you
need to win a round of buzzword bingo.

## The API

A hosted version of this API is available at https://api.ef.gy/fortune, and the
examples here will refer to that base URL.

The API server is also available through a Tor Hidden Service at
http://664ov6iyvgpe63xn.onion/fortune - if you wish to use this service instead
of the regular one, just change the URLs below accordingly. This means your
`curl` programme needs to be set up for Tor access, of course.

Otherwise, you can play along at home with this JSFiddle:
https://jsfiddle.net/jyujin/t87whwjr/62/embed/js,html,css,result/dark/

### GET /fortune

The `/fortune` endpoint looks up a random fortune cookie and returns it as the
resource body of the request. This method will always reply with a code `307`
and a `Location` header which points to the canonical URL for the cookie.

The body of the response still includes the same content you'd get by following
the redirect, as those are fairly short and there didn't seem to be a need to
complicate things further.

#### Basic example

If you requested this resource, like so:

    curl https://api.ef.gy/fortune

The reply you get should be something like this (the actual cookie is chosen at
random):

    Better living a beggar than buried an emperor.

This is the most straightforward use of the API possible: get a random fortune
cookie, no thrills, no strings attached. In fact, if you're on OSX (or didn't
feel like installing `fortune` for some other reason), you can just stick this
in your `.profile`:

    alias fortune="curl https://api.ef.gy/fortune"

And now you don't need to install `fortune` anymore, you can just use this.
Happyness.

### GET /fortune/*ID*

Like `/fortune` but you get to specify the *ID* number of the cookie. This is
nice for linking to specific fortune cookies, as one might want to. This is also
how the `/fortune` endpoint points to a canonical URL in its `Location` header.

IDs are not necessarily stable over time, as they depend on the loading order of
the cookies, which may change over time. It is usually stable, however.

#### Basic example

Assuming you wanted to fetch whatever cookie has ID #42, you could run this
command:

    curl https://api.ef.gy/fortune/42

And you might get a result such as this one:

    At ebb tide I wrote a line upon the sand, and gave it all my heart and all
    my soul.  At flood tide I returned to read what I had inscribed and found my
    ignorance upon the shore.
        -- Kahlil Gibran

These links are usually stable for short-ish intervals, but they may still
change over time.

### Selecting a different output format with the `Accept` header

The server supports output in plain text, XML and JSON formats. You select which
of the three you want by providing a valid `Accept` header using the correct
MIME type of the format you want:

 * `text/plain` selects plain text, and is the default if none is specified
    (for the benefit of `curl` users).
 * `text/json` selects JSON output.
 * `text/xml` and `application/xml` select XML output.

Format selection works for both the `/fortune` and the `/fortune/*ID*`
endpoints, and indeed the output of the two is the same for the same cookie.

#### JSON format

The JSON format is a top-level object with these fields:

 * `cookie`, the actual fortune cookie itself.
 * `file`, the source file of the cookie.
 * `file-id`, the sequence number of the cookie in the `file`.
 * `id`, the global ID of the cookie.

`file` and `file-id` are purely informative, though you could use this
information to look up the cookie in a local copy of the file.

#### XML format

Like the JSON format, this one is pretty straightforward. It's a simple XML
document, with one `<fortune/>` element. The namespace is set as
`http://ef.gy/2012/fortune`, as one does not simply create an XML document
without a namespace.

The cookie itself is the text content of this element, and it has the following
attributes:

 * `sourceFile`, which is the local path the cookie was read from.
 * `fileID`, the sequence number of the cookie in this file.
 * `id`, the global ID of this cookie.

See below for an example of this in actual usage.

### Advanced examples

If we run `curl` with header output enabled...

    curl -i https://api.ef.gy/fortune

We see that the server announces that it does actually check the `Accept` header
and will vary its output accordingly:

    HTTP/1.1 307 Temporary Redirect
    Server: nginx/1.6.2
    Date: Sun, 04 Jun 2017 16:22:58 GMT
    Content-Type: text/plain
    Content-Length: 70
    Connection: keep-alive
    Location: /fortune/12814
    Vary: Accept
    Strict-Transport-Security: max-age=10886400; includeSubDomains; preload
    
    Booker's Law:
      An ounce of application is worth a ton of abstraction.

Also note the `Location` header while you're at it, and the `307` status code
for the `/fortune` endpoint.

Assume we wanted to use this API in a shiny AJAX application. To do so, we'd
naturally want JSON output:

    curl -i https://api.ef.gy/fortune -H 'Accept: text/json'

This produces a result much like this one:

    HTTP/1.1 307 Temporary Redirect
    Server: nginx/1.6.2
    Date: Mon, 05 Jun 2017 11:31:36 GMT
    Content-Type: text/json
    Content-Length: 153
    Connection: keep-alive
    Location: /fortune/11833
    Vary: Accept
    Strict-Transport-Security: max-age=10886400; includeSubDomains; preload
    Access-Control-Allow-Origin: *

    {"cookie":"I'm a Hollywood writer; so I put on a sports jacket and take off my brain.\n","file":"/usr/share/games/fortunes/art","file-id":155,"id":11833}

As mentioned before, this also works when given an ID:

    curl -i https://api.ef.gy/fortune/666 -H 'Accept: text/json'

Which results in...

    HTTP/1.1 200 OK
    Server: nginx/1.6.2
    Date: Mon, 05 Jun 2017 11:30:45 GMT
    Content-Type: text/json
    Content-Length: 271
    Connection: keep-alive
    Vary: Accept
    Strict-Transport-Security: max-age=10886400; includeSubDomains; preload
    Access-Control-Allow-Origin: *

    {"cookie":"<NeonKttn> I had a friend stick me in her closet during highschool beacuse I\n           wouldn't believe that her boyfriend knew about foreplay...\n<NeonKttn> I shoulda brought popcorn. :)\n","file":"/usr/share/games/fortunes/knghtbrd","file-id":167,"id":666}

If you're feeling particularly much like punishing yourself, here's an example
of the XML output, which you would retrieve like this:

    curl https://api.ef.gy/fortune -H 'Accept: text/xml'

... and which would result in...

    <?xml version='1.0' encoding='utf-8'?><fortune xmlns='http://ef.gy/2012/fortune' sourceFile='/usr/share/games/fortunes/politics' fileID='207' id='1786'><![CDATA[I was appalled by this story of the destruction of a member of a valued
    endangered species.  It's all very well to celebrate the practicality of
    pigs by ennobling the porcine sibling who constructed his home out of
    bricks and mortar.  But to wantonly destroy a wolf, even one with an
    excessive taste for porkers, is unconscionable in these ecologically
    critical times when both man and his domestic beasts continue to maraud
    the earth.
        Sylvia Kamerman, "Book Reviewing"
    ]]></fortune>

It's fairly readable XML, but it's still XML.

## Compiling and installing locally

You can compile and run `fortuned` yourself, all you need is a C++14-compatible
compiler and libc on a UNIX-like OS. The instructions for CXXHTTP should get you
started with any dependencies you need: https://github.com/ef-gy/cxxhttp

In addition to those listed there, you'll also need some fortune cookie files.
Installing fortune-mod and a few of the fortune packs should take care of that.

