require 'genx'
f = io.open('test.xml', 'w')
d = genx.new(f)

d:startElement('document')

d:startElement('author')
d:attribute('awesome', 'true')
d:text('Adrian')
d:endElement()

d:startElement('body')
d:text('Hello, everybody')
d:comment('how are you?')
d:endElement()

d:endElement()

d:close()
f:close()