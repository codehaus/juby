
gem: Juby-1.0.0.gem

Juby-1.0.0.gem: juby.gemspec
	ruby juby.gemspec

test: Juby-1.0.0.gem
	mkdir -p $(PWD)/tmp
	gem install Juby-1.0.0.gem --install-dir=$(PWD)/tmp -t 

install: Juby-1.0.0.gem
	gem install Juby-1.0.0.gem -t

uninstall:
	gem uninstall Juby

clean:
	rm -Rf $(PWD)/tmp
	rm -f Juby-1.0.0.gem
