from fastapi import FastAPI
import uvicorn
import requests
from bs4 import BeautifulSoup
import re

rss_feed_url = "https://partner-feeds.publishing.tamedia.ch/rss/tagesanzeiger/news-heute"  # Example RSS feed URL

# scrape website

def get_first_rss_article_content():
    try:
        # Scrape article content
        article_url = "https://www.tagesanzeiger.ch/28-juli-2025-das-wichtigste-des-tages-677287671830"
        article_response = requests.get(article_url, timeout=30)
        article_response.raise_for_status()
        # get text content
        soup = BeautifulSoup(article_response.content, "lxml")
        article_elements = soup.find_all(class_='ArticleElement_article-element__q93eL')
        content = "\n".join([element.get_text() for element in article_elements])
        return {"news": content}
    
    except Exception as e:
        error_msg = f"Error: {e}"
        print error_msg
        return ""

# REST server

app = FastAPI()

@app.get("/api/text")
def get_text():
    return {"message": "Hello from FastAPI!"}

@app.get("/api/text/{name}")
def get_personalized_text(name: str):
    return {"message": f"Hello, {name}!"}

@app.get("/api/dailynews")
def get_dailynews():
    return get_first_rss_article_content()

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)



# import requests
# import xml.etree.ElementTree as ET
# from bs4 import BeautifulSoup
# import re

# # Configuration
# RSS_URL = "https://example.com/feed.xml"  # Replace with your RSS feed URL

# def clean_text(text):
#     """Clean and format text content"""
#     text = re.sub(r'\n\s*\n', '\n\n', text)
#     text = re.sub(r' +', ' ', text)
#     return text.strip()

# def get_first_rss_article_content():
#     try:
#         # Fetch RSS feed
#         logger.info(f"Fetching RSS feed from {RSS_URL}")
#         rss_response = requests.get(RSS_URL, timeout=30)
#         rss_response.raise_for_status()
        
#         # Parse RSS XML
#         root = ET.fromstring(rss_response.content)
        
#         # Get first item only
#         item = root.find('.//item')
        
#         if item is None:
#             return "No articles found in RSS feed"
        
#         title = item.find('title')
#         link = item.find('link')
        
#         if title is None or link is None:
#             return "Invalid RSS item - missing title or link"
        
#         article_title = title.text
#         article_url = link.text
        
#         logger.info(f"Scraping: {article_title}")
#         logger.info(f"URL: {article_url}")
        
#         # Scrape article content
#         article_response = requests.get(article_url, timeout=30)
#         article_response.raise_for_status()
        
#         soup = BeautifulSoup(article_response.content, "lxml")
        
#         # Remove script and style elements
#         for script in soup(["script", "style"]):
#             script.decompose()
        
#         # Get text content
#         text_content = soup.get_text()
#         cleaned_content = clean_text(text_content)
        
#         return cleaned_content
        
#     except Exception as e:
#         error_msg = f"Error: {e}"
#         logger.error(error_msg)
#         return error_msg

# # Execute and return content
# content = get_first_rss_article_content()
# logger.info("Content retrieved successfully")
