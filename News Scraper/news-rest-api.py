from fastapi import FastAPI
import uvicorn
import requests
from bs4 import BeautifulSoup
import re

rss_feed_url = "https://partner-feeds.publishing.tamedia.ch/rss/tagesanzeiger/news-heute"  # Example RSS feed URL
article_title = ""
article_date = ""
article_content = ""

# scrape website

def get_rss_article_content():
    global article_title, article_date, article_content
    try:
        # get article link from RSS feed
        print(f"Fetching RSS feed from {rss_feed_url}")
        rss_response = requests.get(rss_feed_url, timeout=30)
        rss_response.raise_for_status()
        soup = BeautifulSoup(rss_response.content, "lxml-xml")
        guid = soup.guid
        if not guid:
            # no news > return old news
            print("Currently no news, returning previous news.")
            return {"title": article_title, "date": article_date, "news": article_content}
        article_url = soup.guid.text
        #article_url = soup.find_all('link')[2].text   # get third link from RSS feed
        article_title = soup.find_all('description')[1].text
        article_date = soup.pubDate.text
        print(f"Found article URL: {article_url}")
        # scrape article content
        print(f"Scraping article from {article_url}")
        article_response = requests.get(article_url, timeout=30)
        article_response.raise_for_status()
        # get text content
        soup = BeautifulSoup(article_response.content, "lxml")
        article_elements = soup.find_all(class_='ArticleElement_article-element__q93eL')
        article_content = "\n".join([element.get_text() for element in article_elements])
        return {"title": article_title, "date": article_date, "news": article_content}
    
    except Exception as e:
        error_msg = f"Error: {e}"
        print(error_msg)
        return error_msg

# REST server

app = FastAPI()

@app.get("/api/version")
def get_text():
    return {"version": "v1.1"}

#@app.get("/api/text/{name}")
#def get_personalized_text(name: str):
#    return {"message": f"Hello, {name}!"}

@app.get("/api/dailynews")
def get_dailynews():
    return get_rss_article_content()

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)
