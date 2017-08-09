Attribute VB_Name = "PPT2PDF"
'Copyright(c) 2017-2100.  jielong.lin@qq.com.    All rights reserved.

Sub ppt2pdf()

    Dim pdfName As String
    

    If Len(ActivePresentation.Path) <= 0 Then
        MsgBox "Not found the legal file path", vbOKOnly
        Exit Sub
    End If
        
    If Dir(ActivePresentation.Path & "wwwroot\", vbDirectory) = "" Then
        If vbYes = MsgBox("Not found " & ActivePresentation.Path & "wwwroot\, Create it ? ", vbYesNo, "JLL: Warning") Then
            MkDir ActivePresentation.Path & "wwwroot\"
        Else
            Exit Sub
        End If
    End If

    pdfName = ActivePresentation.Path & "wwwroot\" & ActivePresentation.Name
    
    Dim fileSuffix As Variant
    fileSuffix = Array(".ppt", ".pptx")
    Dim i As Integer
    i = 0
    Do While i < 2
        If fileSuffix(i) = Right(pdfName, Len(fileSuffix(i))) Then
            'MsgBox "Hit " & fileSuffix(i)
            'MsgBox Left(pdfName, Len(pdfName) - Len(fileSuffix(i)))
            pdfName = Left(pdfName, Len(pdfName) - Len(fileSuffix(i))) & ".pdf"
            Exit Do
        End If
        MsgBox "Probing which file suffix for " & fileSuffix(i) & " --- " & pdfName, vbOKOnly, "JLL: Info"
        i = i + 1
    Loop
    
    MsgBox "Retrieved target FileName=" & pdfName, vbOKOnly, "JLL: Info"
    If Dir(pdfName) <> "" Then
        Kill pdfName
    End If

    ActivePresentation.PrintOptions.Ranges.Add 1, ActivePresentation.Slides.Count
    ActivePresentation.PrintOptions.RangeType = ppPrintAll
    
    ActivePresentation.ExportAsFixedFormat pdfName, ppFixedFormatTypePDF, ppFixedFormatIntentScreen, msoTrue, ppPrintHandoutHorizontalFirst, ppPrintOutputSlides, msoFalse, ActivePresentation.PrintOptions.Ranges(1), ppPrintAll, "Slideshow Name", False, False, False, False, False


End Sub