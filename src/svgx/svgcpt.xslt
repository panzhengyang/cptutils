<?xml version="1.0"?><!-- -*- sgml -*- -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:strip-space elements="*"/>
  <xsl:preserve-space elements="stop"/>
  <xsl:output method="text"/>

  <xsl:template match="/">
    <xsl:apply-templates select="svg"/>
  </xsl:template>

  <xsl:template match="svg">
    <xsl:apply-templates select="linearGradient"/>
  </xsl:template>

  <xsl:template match="linearGradient">
    <xsl:apply-templates select="stop"/>
  </xsl:template>

  <xsl:template match="stop">
    <xsl:value-of select="@offset"/>
    <xsl:value-of select="@style"/>
  </xsl:template>

</xsl:stylesheet>
