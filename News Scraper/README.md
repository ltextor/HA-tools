Build and start container with
```
$ sudo docker build -t news_scraper .
# interactive for testing
$ sudo docker run -it --rm -p 8000:8000 --name=news_scraper news_scraper
# detached with restart policy for deplyoment
$ sudo docker run -d --restart=unless-stopped -p 8000:8000 --name=news_scraper news_scraper
```

You should have a .env file like the following (or set the environment variables otherwise)
```
GOOGLE_MAPS_API_KEY=your_actual_google_maps_api_key_here
ORIGIN_ADDRESS="Your Starting Address, City, Country"
RSS_URL="https://partner-feeds.publishing.tamedia.ch/rss/tagesanzeiger/news-heute"
```