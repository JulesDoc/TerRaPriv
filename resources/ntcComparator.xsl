<?xml version='1.0'?>
<xsl:stylesheet version="1.0" xmlns:comp_ns="http://TerRaSys/ntcComparator" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
      <xsl:template match="/">
            <html>
                  <body>
                        <table width="100%" cellpadding="2" border="1">
                              <tr bgcolor="lightgreen">
                                    <th>Key</th>
                                    <th>Notice 1</th>
                                    <th>Notice 2</th>
                              </tr>
                              <xsl:for-each select="enotice/*">
                                    <xsl:call-template name="section">
                                          <xsl:with-param name="indent">1</xsl:with-param>
                                    </xsl:call-template>
                              </xsl:for-each>
                        </table>
                  </body>
            </html>
      </xsl:template>
      <!-- section template -->
      <xsl:template name="section">
            <xsl:param name="indent"/>
            <xsl:choose>
                  <!-- Check recursively that the section  have distinct values witch worth to be printed -->
                  <xsl:when test=".//comp_ns:value1 or .//comp_ns:value2">
                        <!-- Print section name -->
                        <tr bgcolor="#F0F0F0">
                              <td style="font-weight:bold;">
                                    <xsl:attribute name="style">padding-left:<xsl:value-of select="$indent"/>
                                    </xsl:attribute>&lt;<xsl:value-of select="name()"/>
                                    <xsl:call-template name="print_attrs"/>></td>
                              <td>
                                    <xsl:if test="not(@comp_ns:index1)">None</xsl:if>
                                    <xsl:if test="@comp_ns:pos1">
                                          <xsl:value-of select="name()"/>
                                          <xsl:text/>
                                          <xsl:value-of select="@comp_ns:pos1"/>
                                    </xsl:if>
                              </td>
                              <td>
                                    <xsl:if test="not(@comp_ns:index2)">None</xsl:if>
                                    <xsl:if test="@comp_ns:pos2">
                                          <xsl:value-of select="name()"/>
                                          <xsl:text/>
                                          <xsl:value-of select="@comp_ns:pos2"/>
                                    </xsl:if>
                              </td>
                        </tr>
                        <!-- Print keys -->
                        <xsl:for-each select="child::*[not(@comp_ns:index1) and not(@comp_ns:index2)]">
                              <xsl:call-template name="key">
                                    <xsl:with-param name="indent">
                                          <xsl:value-of select="number($indent)+20"/>
                                    </xsl:with-param>
                              </xsl:call-template>
                        </xsl:for-each>
                        <!-- Print sub-sections: recursive call  -->
                        <xsl:for-each select="child::*[@comp_ns:index1 or @comp_ns:index2]">
                              <xsl:call-template name="section">
                                    <xsl:with-param name="indent">
                                          <xsl:value-of select="number($indent)+20"/>
                                    </xsl:with-param>
                              </xsl:call-template>
                        </xsl:for-each>
                        <!-- Print end section name -->
                        <tr bgcolor="#F0F0F0">
                              <td style="font-weight:bold;">
                                    <xsl:attribute name="style">padding-left:<xsl:value-of select="$indent"/>
                                    </xsl:attribute>&lt;/<xsl:value-of select="name()"/>></td>
                        </tr>
                  </xsl:when>
            </xsl:choose>
      </xsl:template>
      <!-- key template -->
      <xsl:template name="key">
            <xsl:param name="indent"/>
            <tr>
                  <!-- Check that the values are numeric before performing the comparison -->
                  <xsl:choose>
                        <xsl:when test="number(comp_ns:value1) and number(comp_ns:value2)">
                              <xsl:if test="number(comp_ns:value1)!=number(comp_ns:value2)">
                                    <xsl:attribute name="style">color:red;</xsl:attribute>
                              </xsl:if>
                        </xsl:when>
                        <xsl:otherwise>
                              <xsl:if test="comp_ns:value1!=comp_ns:value2">
                                    <xsl:attribute name="style">color:red;</xsl:attribute>
                              </xsl:if>
                        </xsl:otherwise>
                  </xsl:choose>
                  <td>
                        <xsl:attribute name="style">padding-left:<xsl:value-of select="$indent"/>
                        </xsl:attribute>
                        <xsl:value-of select="name()"/>
                        <xsl:call-template name="print_attrs"/>
                  </td>
                  <td>
                        <xsl:value-of select="comp_ns:value1"/>
                  </td>
                  <td>
                        <xsl:value-of select="comp_ns:value2"/>
                  </td>
            </tr>
      </xsl:template>
      <!-- print_attrs template: print the attributes of the current node -->
      <xsl:template name="print_attrs">
            <xsl:for-each select="@*">
                  <xsl:if test="namespace-uri()!=namespace-uri-for-prefix('comp_ns',..)">
                        <xsl:text/>
                        <xsl:value-of select="name()"/>=<xsl:value-of select="."/>
                  </xsl:if>
            </xsl:for-each>
      </xsl:template>
</xsl:stylesheet>
